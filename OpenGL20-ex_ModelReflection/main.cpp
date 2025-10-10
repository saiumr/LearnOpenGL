#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "vertex.h"
#include "shader.h"
#include "camera.h"
#include "model.h"
#include <stb_image.h>

int  InitWindow();
void RenderLoop();
void Terminate();

unsigned int LoadCubemap(const std::vector<std::string>& faces);
void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void MouseCallback(GLFWwindow* window, double xposIn, double yposIn);
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void ProcessInput(GLFWwindow* window);

GLFWwindow* window { nullptr };
const unsigned int kScreenWidth { 800 };
const unsigned int kScreenHeight{ 600 };

Camera camera{ glm::vec3 {0.0f, 1.5f, 6.0f} };
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
	Terminate();

	return 0;
}

void RenderLoop() {
	GLint max_render_items_count;
	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &max_render_items_count);
	std::cout << "max render items count: " << max_render_items_count << std::endl;  // 192

	std::string base_dir{ "skybox/default/" };
	std::vector<std::string> faces{
		base_dir + "right.jpg",
		base_dir + "left.jpg",
		base_dir + "top.jpg",
		base_dir + "bottom.jpg",
		base_dir + "front.jpg",
		base_dir + "back.jpg"
	};
	unsigned int cubemap_texture{ LoadCubemap(faces) };

	Shader shader("model_loading.vert", "model_loading.frag");
	Model  model_obj("nanosuit_reflection/nanosuit.obj");

	Vertex vertex{};
	Shader skybox_shader{ "skybox.vert", "skybox.frag" };
	skybox_shader.use();
	skybox_shader.setInt("skybox", 0); // set GL_TEXTURE0 to skybox firstly

	// draw in wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	while (!glfwWindowShouldClose(window)) {
		float current_frame = static_cast<float>(glfwGetTime());
		delta_time = current_frame - last_frame;
		last_frame = current_frame;

		ProcessInput(window);

		glClearColor(0.15f, 0.16f, 0.18f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// don't forget to enable shader before setting uniforms
		shader.use();

		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)kScreenWidth / (float)kScreenHeight, 0.1f, 100.0f);
		
		shader.setMat4("projection", projection);
		shader.setMat4("view", view);

		// render the loaded model
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));	// it's a bit too big for our scene, so scale it down
		shader.setMat4("model", model);
		model_obj.Draw(shader);

		// draw skybox as last
		glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
		skybox_shader.use();
		view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
		skybox_shader.setMat4("view", view);
		skybox_shader.setMat4("projection", projection);
		// skybox cube
		glBindVertexArray(vertex.skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_texture);
		vertex.Draw(vertex.skyboxVAO);
		glDepthFunc(GL_LESS); // set depth function back to default

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	shader.Clean();
}

int InitWindow() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	window = glfwCreateWindow(kScreenWidth, kScreenHeight, "Advanced: ex Model reflection", nullptr, nullptr);
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

	// tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
	//stbi_set_flip_vertically_on_load(true);

	return 0;
}

void Terminate() {
	glfwTerminate();
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

void ProcessInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camera.ProcessKeyboard(kForward, delta_time);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camera.ProcessKeyboard(kBackward, delta_time);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		camera.ProcessKeyboard(kLeft, delta_time);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		camera.ProcessKeyboard(kRight, delta_time);
	}

	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
		camera.ProcessKeyboard(kUpward, delta_time);
	}
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
		camera.ProcessKeyboard(kDownward, delta_time);
	}

}

// loads a cubemap texture from 6 individual texture faces
// order:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front) 
// -Z (back)
// -------------------------------------------------------
unsigned int LoadCubemap(const std::vector<std::string>& faces) {
	unsigned int textureID;
	glGenBuffers(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++) {
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else {
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}
