#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "vertex.h"
#include "shader.h"
#include "camera.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using TEXTURE = unsigned int;

int InitWindow();
void RenderLoop();

TEXTURE CreateTexture(const char* file_path);
void ProcessInput(GLFWwindow* window);
void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void MouseCallback(GLFWwindow* window, double xposIn, double yposIn);
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

GLFWwindow* window = nullptr;
const int kScreenWidth = 800;
const int kScreenHeight = 600;
// lighting
glm::vec3 light_pos{1.2f, 1.0f, 2.0f};

Camera camera{ glm::vec3 {0.0f, 1.5f, 6.0f} };
bool firstMouse = true;
float lastX = static_cast<float>(kScreenWidth) / 2.0f;
float lastY = static_cast<float>(kScreenHeight) / 2.0f;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main(int argc, const char** argv) {
	if (InitWindow() < 0) {
		return -1;
	}

	RenderLoop();
	glfwTerminate();

	return 0;
}


//////////////////////////////////////////////////////////////


int InitWindow() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	window = glfwCreateWindow(kScreenWidth, kScreenHeight, "Lighting: Multiple Lights", nullptr, nullptr);
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

void RenderLoop() {
	GLint max_render_items_count;
	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &max_render_items_count);
	std::cout << "max render items count: " << max_render_items_count << std::endl;  // 192

	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, CreateTexture("container2.png"));
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, CreateTexture("container2_specular.png"));

	Vertex vertex;
	Shader shader{"multiple_light.vert", "multiple_light.frag"};
	Shader light_shader{"light_cube.vert", "light_cube.frag"};

	shader.use();
	shader.setInt("material.diffuse", 0);
	shader.setInt("material.specular", 1);

	// positions all containers
	glm::vec3 boxes_positions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};
	// positions of the point lights
	glm::vec3 point_light_positions[] = {
		glm::vec3(0.7f,  0.2f,  2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3(0.0f,  0.0f, -3.0f)
	};

	while (!glfwWindowShouldClose(window)) {
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		ProcessInput(window);

		glClearColor(0.15f, 0.16f, 0.18f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.use();
		shader.setVec3("view_pos", camera.Position);

		// material properties
		shader.setFloat("material.shininess", 0.25f * 128.0f);

		// directional light
		shader.setVec3("direction_light.direction", -0.2f, -1.0f, -0.3f);  // upward
		shader.setVec3("direction_light.ambient", 0.05f, 0.05f, 0.05f);
		shader.setVec3("direction_light.diffuse", 0.4f, 0.4f, 0.4f);
		shader.setVec3("direction_light.specular", 0.5f, 0.5f, 0.5f);
		// point light 1
		shader.setVec3("point_lights[0].position", point_light_positions[0]);
		shader.setVec3("point_lights[0].ambient", 0.05f, 0.05f, 0.05f);
		shader.setVec3("point_lights[0].diffuse", 0.8f, 0.8f, 0.8f);
		shader.setVec3("point_lights[0].specular", 1.0f, 1.0f, 1.0f);
		shader.setFloat("point_lights[0].constant", 1.0f);
		shader.setFloat("point_lights[0].linear", 0.09f);
		shader.setFloat("point_lights[0].quadratic", 0.032f);
		// point light 2
		shader.setVec3("point_lights[1].position", point_light_positions[1]);
		shader.setVec3("point_lights[1].ambient", 0.05f, 0.05f, 0.05f);
		shader.setVec3("point_lights[1].diffuse", 0.8f, 0.8f, 0.8f);
		shader.setVec3("point_lights[1].specular", 1.0f, 1.0f, 1.0f);
		shader.setFloat("point_lights[1].constant", 1.0f);
		shader.setFloat("point_lights[1].linear", 0.09f);
		shader.setFloat("point_lights[1].quadratic", 0.032f);
		// point light 3
		shader.setVec3("point_lights[2].position", point_light_positions[2]);
		shader.setVec3("point_lights[2].ambient", 0.05f, 0.05f, 0.05f);
		shader.setVec3("point_lights[2].diffuse", 0.8f, 0.8f, 0.8f);
		shader.setVec3("point_lights[2].specular", 1.0f, 1.0f, 1.0f);
		shader.setFloat("point_lights[2].constant", 1.0f);
		shader.setFloat("point_lights[2].linear", 0.09f);
		shader.setFloat("point_lights[2].quadratic", 0.032f);
		// point light 4
		shader.setVec3("point_lights[3].position", point_light_positions[3]);
		shader.setVec3("point_lights[3].ambient", 0.05f, 0.05f, 0.05f);
		shader.setVec3("point_lights[3].diffuse", 0.8f, 0.8f, 0.8f);
		shader.setVec3("point_lights[3].specular", 1.0f, 1.0f, 1.0f);
		shader.setFloat("point_lights[3].constant", 1.0f);
		shader.setFloat("point_lights[3].linear", 0.09f);
		shader.setFloat("point_lights[3].quadratic", 0.032f);
		// spotLight
		shader.setVec3("spotlight.position", camera.Position);
		shader.setVec3("spotlight.direction", camera.Front);
		shader.setVec3("spotlight.ambient", 0.2f, 0.2f, 0.2f);
		shader.setVec3("spotlight.diffuse", 1.0f, 1.0f, 1.0f);
		shader.setVec3("spotlight.specular", 1.0f, 1.0f, 1.0f);
		shader.setFloat("spotlight.constant", 1.0f);
		shader.setFloat("spotlight.linear", 0.09f);
		shader.setFloat("spotlight.quadratic", 0.032f);
		shader.setFloat("spotlight.inner_cut_off", glm::cos(glm::radians(12.5f)));
		shader.setFloat("spotlight.outer_cut_off", glm::cos(glm::radians(15.0f)));

		// container object
		glm::mat4 model{ 1.0f };
		glm::mat4 view{ 1.0f };
		glm::mat4 projection{ 1.0f };
		view = camera.GetViewMatrix();
		projection = glm::perspective(glm::radians(camera.Zoom), (float)kScreenWidth / kScreenHeight, 0.1f, 100.0f);
		shader.setMat4("model", model);
		shader.setMat4("view", view);
		shader.setMat4("projection", projection);

		glBindVertexArray(vertex.get_VAO());
		for (int i = 0; i < 10; ++i) {
			glm::mat4 model{ glm::mat4{ 1.0f } };
			model = glm::translate(model, boxes_positions[i]);
			float angle { 20.0f * i };
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			shader.setMat4("model", model);

			// 计算法向量矩阵（模型矩阵的逆转置）
			glm::mat3 normalMatrix{ glm::transpose(glm::inverse(glm::mat3(model))) };
			shader.setMat3("normalMatrix", normalMatrix);

			glDrawElements(GL_TRIANGLES, vertex.get_ElementCount(), GL_UNSIGNED_INT, 0);
		}

		// light cube
		light_shader.use();
		light_shader.setMat4("model", model);
		light_shader.setMat4("view", view);
		light_shader.setMat4("projection", projection);

		glBindVertexArray(vertex.get_LightVAO());
		for (unsigned int i = 0; i < 4; i++)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, point_light_positions[i]);
			model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
			light_shader.setMat4("model", model);
			glDrawElements(GL_TRIANGLES, vertex.get_ElementCount(), GL_UNSIGNED_INT, 0);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	shader.Clean();
	light_shader.Clean();
	vertex.Clean();
}

TEXTURE CreateTexture(const char* file_path) {
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	// Texture Wrapping
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// Texture Filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(file_path, &width, &height, &nrChannels, 0);
	//std::cout << "w: " << width << ", h: " << height << std::endl;
	if (data) {
		if (std::strstr(file_path, ".png") != nullptr) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
		else {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	return texture;
}

void ProcessInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(kForward, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(kBackward, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(kLeft, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(kRight, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_X))
		camera.ProcessKeyboard(kUpward, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_Z))
		camera.ProcessKeyboard(kDownward, deltaTime);

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

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}