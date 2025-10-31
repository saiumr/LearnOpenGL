#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <map>
#include <memory>
#include <array>
#include "vertex.h"
#include "shader.h"
#include "camera.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int  InitWindow();
void RenderLoop();

unsigned int LoadTexture(const char* file_path, bool gamma_correction);
void ProcessInput(GLFWwindow* window);
void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void MouseCallback(GLFWwindow* window, double xposIn, double yposIn);
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

GLFWwindow* window { nullptr };
const int kScreenWidth  { 800 };
const int kScreenHeight { 600 };

Camera camera { glm::vec3 {0.0f, 0.0f, 5.0f} };
bool  first_mouse { true };
float last_x { static_cast<float>(kScreenWidth) / 2.0f };
float last_y { static_cast<float>(kScreenHeight) / 2.0f };
float delta_time { 0.0f };
float last_frame { 0.0f };

int main(int argc, const char** argv) {
	if (InitWindow() < 0) {
		return -1;
	}

	RenderLoop();
	glfwTerminate();

	return 0;
}


//////////////////////////////////////////////////////////////


void RenderLoop() {
	Vertex vertex;
	unsigned int floor_texture { LoadTexture("wood.jpg", true) };
	unsigned int box_texture   { LoadTexture("box.jpg", true) };
	Shader shader { "point_shadows.vert", "point_shadows.frag" };
	Shader light_shader { "light_cube.vert", "light_cube.frag" };
	Shader simple_depth_shader { "point_shadows_depth.vert", "point_shadows_depth.frag", "point_shadows_depth.geom" };

	shader.use();
	shader.setInt("diffuse_texture", 0);
	shader.setInt("depthMap", 1);

	glm::vec3 light_pos { 0.0f, 0.0f, 0.0f };

	std::array<glm::vec3, 6> boxes {
		glm::vec3{ 0.0f,  1.5f,  3.0f},
		glm::vec3{ 5.0f,  2.0f,  1.0f},
		glm::vec3{-2.0f, -3.0f,  1.0f},
		glm::vec3{-4.0f,  3.0f, -2.0f},
		glm::vec3{ 3.0f, -2.0f,  1.0f},
		glm::vec3{ 2.0f,  2.5f, -1.0f},
	};

	// create depth map fbo
	const int kShadowWidth{ 1024 }, kShadowHeight{ 1024 };
	unsigned int depth_map_fbo;
	glGenFramebuffers(1, &depth_map_fbo);
	unsigned int depth_cube_map;
	glGenTextures(1, &depth_cube_map);
	glBindTexture(GL_TEXTURE_CUBE_MAP, depth_cube_map);
	for (int i = 0; i < 6; ++i) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, kShadowWidth, kShadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	// attach cubemao to fbo
	glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_cube_map, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0.15f, 0.16f, 0.18f, 1.0f);  // ?

	// for light move
	float radius { 1.5f };
	int  circle_mode { 0 }; // 012 -> circle xyz
	const float kPI { 3.14159f };
	const float kCircleSpeed { 60.0f };  // 60 degrees / s
	float total_angle { 0.0f };

	while (!glfwWindowShouldClose(window)) {
		float current_frame = static_cast<float>(glfwGetTime());
		delta_time = current_frame - last_frame;
		last_frame = current_frame;

		ProcessInput(window);

		glClearColor(0.15f, 0.16f, 0.18f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// light moving
		total_angle += delta_time * kCircleSpeed;
		int current_circle_mode { static_cast<int>(std::floor(total_angle / 270.0f)) % 3 };
		if (circle_mode == 2 && current_circle_mode == 0) {
			total_angle = std::fmod(total_angle, 270.0f * 3.0f);
		}
		circle_mode = current_circle_mode;

		float a { static_cast<float>(std::cos(glm::radians(total_angle))) * radius };
		float b { static_cast<float>(std::sin(glm::radians(total_angle))) * radius };
		if (circle_mode == 0) {  // around x axis
			light_pos.y = a;
			light_pos.z = b;
		} else if (circle_mode == 1) {  // around y axis
			light_pos.z = b;
			light_pos.x = a;
		} else if (circle_mode == 2) {  // around z axis
			light_pos.x = a;
			light_pos.y = b;
		}
		//light_pos = glm::vec3{ 0.0f };
		// set depth map transform matrix
		float aspect{ static_cast<float>(kShadowWidth) / static_cast<float>(kShadowHeight) };
		float near { 1.0f };
		float far { 25.0f };
		glm::mat4 shadow_projection{ glm::perspective(glm::radians(90.0f), aspect, near, far) };  // transform radians
		// 1. 纹理坐标（UV/ST）的 V/T 轴默认向下递增（原点(0,0)在纹理左上角），与笛卡尔坐标系（世界/观察空间）Y轴向上的方向天然相反；
		// 2. 立方体贴图的 ±X、±Z 面将这种“V/T轴向下”特性与观察空间的 -Y 轴强制绑定，这是 OpenGL 规定的固定映射规则；
		// 3. 普通相机渲染到屏幕时，OpenGL 会自动翻转 V/T 轴以匹配人眼直观的“上为正”，无需手动调整 up 向量；
		// 4. 点光源阴影需渲染到立方体贴图，需手动匹配纹理坐标方向，因此 up 向量设为(0,-1,0)，让观察空间 -Y 轴与纹理 V/T 轴方向一致，避免深度图上下颠倒导致阴影错位。
		std::array<glm::mat4, 6> shadow_transforms {
			shadow_projection * glm::lookAt(light_pos, light_pos + glm::vec3(1.0,  0.0,  0.0), glm::vec3(0.0,-1.0,  0.0)),
			shadow_projection * glm::lookAt(light_pos, light_pos + glm::vec3(-1.0, 0.0,  0.0), glm::vec3(0.0,-1.0,  0.0)),
			shadow_projection * glm::lookAt(light_pos, light_pos + glm::vec3(0.0,  1.0,  0.0), glm::vec3(0.0, 0.0,  1.0)),
			shadow_projection * glm::lookAt(light_pos, light_pos + glm::vec3(0.0, -1.0,  0.0), glm::vec3(0.0, 0.0, -1.0)),
			shadow_projection * glm::lookAt(light_pos, light_pos + glm::vec3(0.0,  0.0,  1.0), glm::vec3(0.0,-1.0,  0.0)),
			shadow_projection * glm::lookAt(light_pos, light_pos + glm::vec3(0.0,  0.0, -1.0), glm::vec3(0.0,-1.0,  0.0)),
		};

		// render secene to depth map
		glViewport(0, 0, kShadowWidth, kShadowHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo);
		glClear(GL_DEPTH_BUFFER_BIT);
		simple_depth_shader.use();
		for (int i = 0; i < 6; ++i) {
			simple_depth_shader.setMat4("shadowMatrices[" + std::to_string(i) + "]", shadow_transforms[i]);
		}
		simple_depth_shader.setFloat("far_plane", far);
		simple_depth_shader.setVec3("lightPos", light_pos);
		// render scene
		glm::mat4 model { 1.0f };
		model = glm::scale(model, glm::vec3{ 16.0f });
		simple_depth_shader.setMat4("model", model);
		glBindVertexArray(vertex.cubeVAO);
		vertex.Draw(vertex.cubeVAO);

		glEnable(GL_CULL_FACE);
		glBindVertexArray(vertex.cubeVAO);
		for (const auto& m : boxes) {
			model = glm::mat4{ 1.0f };
			model = glm::translate(model, m);
			simple_depth_shader.setMat4("model", model);
			vertex.Draw(vertex.cubeVAO);
		}
		glDisable(GL_CULL_FACE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// reset renderer status
		glViewport(0, 0, kScreenWidth, kScreenHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// render normal scene
		model = glm::mat4 { 1.0f };
		glm::mat4 view  { camera.GetViewMatrix() };
		glm::mat4 projection{ glm::perspective(glm::radians(45.0f), static_cast<float>(kScreenWidth) / static_cast<float>(kScreenHeight), 0.1f, 100.0f) };

		shader.use();
		// container
		model = glm::scale(model, glm::vec3{ 16.0f });
		shader.setMat4("model", model);
		shader.setMat4("view", view);
		shader.setMat4("projection", projection);
		shader.setVec3("viewPos", camera.Position);
		shader.setVec3("lightPos", light_pos);
		shader.setFloat("far_plane", far);
		shader.setFloat("shininess", 64.0f);
		shader.setBool("reverse_normal", true);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, depth_cube_map);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, floor_texture);
		glBindVertexArray(vertex.cubeVAO);
		vertex.Draw(vertex.cubeVAO);

		// boxes
		shader.setFloat("shininess", 16.0f);
		shader.setBool("reverse_normal", false);
		glEnable(GL_CULL_FACE);
		glBindTexture(GL_TEXTURE_2D, box_texture);
		glBindVertexArray(vertex.cubeVAO);
		for (const auto& m : boxes) {
			model = glm::mat4{ 1.0f };
			model = glm::translate(model, m);
			shader.setMat4("model", model);
			vertex.Draw(vertex.cubeVAO);
		}
		glDisable(GL_CULL_FACE);

		// light cube
		light_shader.use();
		model = glm::mat4{ 1.0f };
		model = glm::translate(model, light_pos);
		model = glm::scale(model, glm::vec3{ 0.2f });
		light_shader.setMat4("model", model);
		light_shader.setMat4("view", view);
		light_shader.setMat4("projection", projection);
		glBindVertexArray(vertex.cubeVAO);
		vertex.Draw(vertex.cubeVAO);

		glBindVertexArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	shader.Clean();
	light_shader.Clean();
	simple_depth_shader.Clean();
}


int InitWindow() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	window = glfwCreateWindow(kScreenWidth, kScreenHeight, "Advanced Lighting: Point Shadows", nullptr, nullptr);
	if (!window) {
		std::cout << "Failed to create GLFW window" << std::endl;
		return -1;
	}
	glfwMakeContextCurrent(window);

	// init glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to init GLAD" << std::endl;
		return -1;
	}

	glEnable(GL_DEPTH_TEST);

	glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
	glfwSetCursorPosCallback(window, MouseCallback);
	glfwSetScrollCallback(window, ScrollCallback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	return 0;
}

unsigned int LoadTexture(const char* file_path, bool gamma_correction) {
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(file_path, &width, &height, &nrComponents, 0);
	if (data) {
		GLenum format{ GL_RGB };
		GLenum internal_format { GL_RGB };
		if (nrComponents == 1)
			internal_format = format = GL_RED;
		else if (nrComponents == 3) {
			internal_format = gamma_correction ? GL_SRGB : GL_RGB;
			format = GL_RGB;
		}
		else if (nrComponents == 4) {
			internal_format = gamma_correction ? GL_SRGB_ALPHA : GL_RGBA;
			format = GL_RGBA;
		}

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		// use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	} else {
		std::cout << "Texture failed to load at path: " << file_path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

void ProcessInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(kForward, delta_time);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(kBackward, delta_time);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(kLeft, delta_time);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(kRight, delta_time);

	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
		camera.ProcessKeyboard(kUpward, delta_time);
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
		camera.ProcessKeyboard(kDownward, delta_time);
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
		std::cout << "camera pos: { "
		<<  camera.Position.x << ", " << camera.Position.y << "," << camera.Position.z << " }"
		<< std::endl;
}

void FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void MouseCallback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (first_mouse)
	{
		last_x = xpos;
		last_y = ypos;
		first_mouse = false;
	}

	float xoffset = xpos - last_x;
	float yoffset = last_y - ypos; // reversed since y-coordinates go from bottom to top

	last_x = xpos;
	last_y = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}