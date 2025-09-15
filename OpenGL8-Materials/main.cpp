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

bool IsEquipCamera = false;

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

	window = glfwCreateWindow(kScreenWidth, kScreenHeight, "Lighting: Ambient light and Difuse light", nullptr, nullptr);
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
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, CreateTexture("container.jpg"));
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, CreateTexture("awesomeface.png"));

	Vertex vertex;
	Shader shader{"shader.vs", "shader.fs"};
	Shader light_shader{"light.vs", "light.fs"};

	shader.use();
	shader.setInt("texture0", 0);
	shader.setInt("texture1", 1);
	shader.setFloat("blend", 0.33f);

	while (!glfwWindowShouldClose(window)) {
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		ProcessInput(window);
		if (IsEquipCamera) {
			// transform light position to camera space
			glm::mat3 camera_rotation = glm::mat3{ camera.Right, camera.Up, -camera.Front };
			light_pos = camera.Position + camera_rotation * glm::vec3{ 0.0f, 0.0f, -2.0f };
		}

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// object
		shader.use();

		glm::mat4 model { 1.0f };
		glm::mat4 view { 1.0f };
		glm::mat4 projection { 1.0f };
		view = camera.GetViewMatrix();
		projection = glm::perspective(glm::radians(camera.Zoom), (float)kScreenWidth/kScreenHeight, 0.1f, 100.0f);
		shader.setMat4("model", model);
		shader.setMat4("view", view);
		shader.setMat4("projection", projection);
		shader.setVec3("light.position", light_pos);
		shader.setVec3("view_pos", camera.Position);

		// light properties
		glm::vec3 lightColor;
		lightColor.x = static_cast<float>(sin(glfwGetTime() * 2.0));
		lightColor.y = static_cast<float>(sin(glfwGetTime() * 0.7));
		lightColor.z = static_cast<float>(sin(glfwGetTime() * 1.3));
		glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f); // decrease the influence
		glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f); // low influence
		shader.setVec3("light.ambient", ambientColor);
		shader.setVec3("light.diffuse", diffuseColor);
		shader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

		// material properties
		shader.setVec3("material.ambient", 1.0f, 0.5f, 0.31f);
		shader.setVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
		shader.setVec3("material.specular", 0.5f, 0.5f, 0.5f); // specular lighting doesn't have full effect on this object's material
		shader.setFloat("material.shininess", 32.0f);

		glBindVertexArray(vertex.get_VAO());
		glDrawElements(GL_TRIANGLES, vertex.get_ElementCount(), GL_UNSIGNED_INT, 0);
		
		// light
		light_shader.use();
		model = glm::mat4{ 1.0f };
		model = glm::translate(model, light_pos);
		model = glm::scale(model, glm::vec3(0.25f, 0.25f, 0.25f));
		light_shader.setMat4("model", model);
		light_shader.setMat4("view", view);
		light_shader.setMat4("projection", projection);

		glBindVertexArray(vertex.get_LightVAO());
		glDrawElements(GL_TRIANGLES, vertex.get_ElementCount(), GL_UNSIGNED_INT, 0);

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

	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		IsEquipCamera = !IsEquipCamera;
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