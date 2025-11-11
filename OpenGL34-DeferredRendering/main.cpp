#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <map>
#include <memory>
#include "vertex.h"
#include "shader.h"
#include "camera.h"
#include "model.h"
#include "stb_image.h"

int  InitWindow();
void RenderLoop();

unsigned int LoadTexture(const char* file_path, bool gamma_correction);
void ProcessInput(GLFWwindow* window);
void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void MouseCallback(GLFWwindow* window, double xposIn, double yposIn);
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
float CalcLightBallRedius(const glm::vec3& light_color, float linear, float quadratic);

GLFWwindow* window{ nullptr };
const int kScreenWidth{ 800 };
const int kScreenHeight{ 600 };

Camera camera{ glm::vec3 {0.0f, 0.0f, 5.0f} };
bool  first_mouse{ true };
float last_x{ static_cast<float>(kScreenWidth) / 2.0f };
float last_y{ static_cast<float>(kScreenHeight) / 2.0f };
float delta_time{ 0.0f };
float last_frame{ 0.0f };

int  select_num{ 2 };
bool left_arrow_pressed{ false };
bool right_arrow_pressed{ false };

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
	Shader shader_geometry_pass { "g_buffer.vert", "g_buffer.frag" };
	Shader shader_lighting_pass { "deferred_shading.vert", "deferred_shading.frag" };
	Shader shader_light_box { "deferred_light_box.vert", "deferred_light_box.frag" };

	shader_lighting_pass.use();
	shader_lighting_pass.setInt("gPosition", 0);
	shader_lighting_pass.setInt("gNormal", 1);
	shader_lighting_pass.setInt("gAlbedoSpec", 2);

	Model object_model { "backpack/backpack.obj" };
	std::vector<glm::vec3> object_positions;
	object_positions.emplace_back(glm::vec3{ -3.0,  -0.5, -3.0 });
	object_positions.emplace_back(glm::vec3{  0.0,  -0.5, -3.0 });
	object_positions.emplace_back(glm::vec3{  3.0,  -0.5, -3.0 });
	object_positions.emplace_back(glm::vec3{ -3.0,  -0.5,  0.0 });
	object_positions.emplace_back(glm::vec3{  0.0,  -0.5,  0.0 });
	object_positions.emplace_back(glm::vec3{  3.0,  -0.5,  0.0 });
	object_positions.emplace_back(glm::vec3{ -3.0,  -0.5,  3.0 });
	object_positions.emplace_back(glm::vec3{  0.0,  -0.5,  3.0 });
	object_positions.emplace_back(glm::vec3{  3.0,  -0.5,  3.0 });

	const unsigned int NR_LIGHTS = 32;
	std::vector<glm::vec3> lightPositions;
	std::vector<glm::vec3> lightColors;
	srand(13);
	for (unsigned int i = 0; i < NR_LIGHTS; i++)
	{
		// calculate slightly random offsets
		float xPos = static_cast<float>(((rand() % 100) / 100.0) * 6.0 - 3.0);
		float yPos = static_cast<float>(((rand() % 100) / 100.0) * 6.0 - 4.0);
		float zPos = static_cast<float>(((rand() % 100) / 100.0) * 6.0 - 3.0);
		lightPositions.push_back(glm::vec3(xPos, yPos, zPos));
		// also calculate random color
		float rColor = static_cast<float>(((rand() % 100) / 200.0f) + 0.5); // between 0.5 and 1.0
		float gColor = static_cast<float>(((rand() % 100) / 200.0f) + 0.5); // between 0.5 and 1.0
		float bColor = static_cast<float>(((rand() % 100) / 200.0f) + 0.5); // between 0.5 and 1.0
		lightColors.push_back(glm::vec3(rColor, gColor, bColor));
	}

	// create g-buffer and config
	unsigned int gBuffer;                           // frame buffer
	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	unsigned int  gPosition, gNormal, gAlbedoSpec;  // color attachments
	// 位置和法线需要更多精度
	// config position color attachment
	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, kScreenWidth, kScreenHeight, 0, GL_RGBA, GL_FLOAT, nullptr);  // 使用GL_RGBA16F 可以存储默认范围 0.0 和 1.0 之外的浮点值
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
	// config normal color attachment
	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, kScreenWidth, kScreenHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
	// config albedospec color attachment
	glGenTextures(1, &gAlbedoSpec);
	glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, kScreenWidth, kScreenHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);

	// tell OpenGL which color attachment we'll use(of this framebuffer) for rendering
	unsigned int attachments[3] { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);  // we could set layout location 0, 1, 2 in fragment shader

	// create and attach depth buffer
	unsigned int rboDepth;
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, kScreenWidth, kScreenHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

	// check completeness
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Framebuffer 1 not complete!" << std::endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	while (!glfwWindowShouldClose(window)) {
		float current_frame = static_cast<float>(glfwGetTime());
		delta_time = current_frame - last_frame;
		last_frame = current_frame;

		ProcessInput(window);

		glClearColor(0.01f, 0.01f, 0.01f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 model{ 1.0f };
		glm::mat4 view{ camera.GetViewMatrix() };
		glm::mat4 projection{ glm::perspective(glm::radians(camera.Zoom), static_cast<float>(kScreenWidth) / static_cast<float>(kScreenHeight), 0.1f, 100.0f) };
		
		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
		glClearColor(0.01f, 0.01f, 0.01f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader_geometry_pass.use();
		shader_geometry_pass.setMat4("view", view);
		shader_geometry_pass.setMat4("projection", projection);
		for (int i = 0; i < object_positions.size(); ++i) {
			model = glm::mat4{ 1.0f };
			model = glm::translate(model, object_positions[i]);
			model = glm::scale(model, glm::vec3(0.5f));
			shader_geometry_pass.setMat4("model", model);
			object_model.Draw(shader_geometry_pass);
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		// now we copy depth info to default framebuffer's depth buffer
		glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer
		glBlitFramebuffer(0, 0, kScreenWidth, kScreenHeight, 0, 0, kScreenWidth, kScreenHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// lighting pass
		// 启用加法混合（累加多个光源的光照贡献）
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE); // 混合模式：新光照颜色 + 已有颜色
		glDepthMask(GL_FALSE);       // 禁用深度写入（避免球体覆盖G-Buffer的深度）
		shader_lighting_pass.use();
		shader_lighting_pass.setInt("select_num", select_num);
		shader_lighting_pass.setVec3("viewPos", camera.Position);
		shader_lighting_pass.setMat4("projection", projection);
		shader_lighting_pass.setMat4("view", view);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gPosition);	
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gNormal);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
		glBindVertexArray(vertex.ballVAO);
		for (unsigned int i = 0; i < lightPositions.size(); i++) {
			auto& light_pos { lightPositions[i] };
			auto& light_color { lightColors[i] };
			// update attenuation parameters and calculate radius
			float linear = 0.7f;
			float quadratic = 1.8f;
			float radius = CalcLightBallRedius(lightColors[i], linear, quadratic);

			shader_lighting_pass.setVec3("light.Position", light_pos);
			shader_lighting_pass.setVec3("light.Color", light_color);
			shader_lighting_pass.setFloat("light.Linear", linear);
			shader_lighting_pass.setFloat("light.Quadratic", quadratic);

			// calculate model
			glm::mat4 model { 1.0f };
			model = glm::translate(model, light_pos);
			model = glm::scale(model, glm::vec3(radius));
			shader_lighting_pass.setMat4("model", model);
			
			vertex.Draw(vertex.ballVAO);
		}
		// 恢复渲染状态
		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);

		// finally render light boxes
		shader_light_box.use();
		shader_light_box.setMat4("view", view);
		shader_light_box.setMat4("projection", projection);
		glBindVertexArray(vertex.cubeVAO);
		for (unsigned int i = 0; i < lightPositions.size(); i++) {
			model = glm::mat4{ 1.0f };
			model = glm::translate(model, lightPositions[i]);
			model = glm::scale(model, glm::vec3(0.1f));
			shader_light_box.setMat4("model", model);
			shader_light_box.setVec3("lightColor", lightColors[i]);
			
			vertex.Draw(vertex.cubeVAO);
		}
		
		// debug code
		//shader_light_box.use();
		//shader_light_box.setMat4("view", view);
		//shader_light_box.setMat4("projection", projection);
		//glBindVertexArray(vertex.ballVAO);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		//for (unsigned int i = 0; i < lightPositions.size(); i++) {
		//	float linear = 0.7f;
		//	float quadratic = 1.8f;
		//	float radius = CalcLightBallRedius(lightColors[i], linear, quadratic);
		//	model = glm::mat4{ 1.0f };
		//	model = glm::translate(model, lightPositions[i]);
		//	model = glm::scale(model, glm::vec3(radius));
		//	shader_light_box.setMat4("model", model);
		//	shader_light_box.setVec3("lightColor", lightColors[i]);
		//	
		//	vertex.Draw(vertex.ballVAO);
		//}
		//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


		glBindVertexArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	shader_geometry_pass.Clean();
	shader_lighting_pass.Clean();
	shader_light_box.Clean();
}


int InitWindow() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	window = glfwCreateWindow(kScreenWidth, kScreenHeight, "Advanced Lighting: Deferred Rendering", nullptr, nullptr);
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
		GLenum internal_format{ GL_RGB };
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
	}
	else {
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

	if (!left_arrow_pressed && glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		select_num = (select_num + 3) % 4;
		left_arrow_pressed = true;

		switch (select_num) {
		case 0:
			std::cout << "position texture" << std::endl;
			break;
		case 1:
			std::cout << "normal texture" << std::endl;
			break;
		case 2:
			std::cout << "diffuse texture" << std::endl;
			break;
		case 3:
			std::cout << "specular gray texture" << std::endl;
			break;
		default:
			break;
		}
	}
	
	if (!right_arrow_pressed && glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		select_num = (select_num + 1) % 4;
		right_arrow_pressed = true;
		switch (select_num) {
		case 0:
			std::cout << "position texture" << std::endl;
			break;
		case 1:
			std::cout << "normal texture" << std::endl;
			break;
		case 2:
			std::cout << "diffuse texture" << std::endl;
			break;
		case 3:
			std::cout << "specular gray texture" << std::endl;
			break;
		default:
			break;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_RELEASE) {
		left_arrow_pressed = false;
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_RELEASE) {
		right_arrow_pressed = false;
	}

	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
		camera.ProcessKeyboard(kUpward, delta_time);
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
		camera.ProcessKeyboard(kDownward, delta_time);
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
		std::cout << "camera pos: { "
		<< camera.Position.x << ", " << camera.Position.y << "," << camera.Position.z << " }"
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

float CalcLightBallRedius(const glm::vec3& light_color, float linear, float quadratic) {
	const float constant = 1.0f; // note that we don't send this to the shader, we assume it is always 1.0 (in our case)
	// then calculate radius of light volume/sphere
	const float maxBrightness = std::fmaxf(std::fmaxf(light_color.r, light_color.g), light_color.b);
	float radius = (-linear + std::sqrt(linear * linear - 4 * quadratic * (constant - (256.0f / 5.0f) * maxBrightness))) / (2.0f * quadratic);
	return radius;
}
