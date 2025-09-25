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

int  InitWindow();
void RenderLoop();

unsigned int LoadTexture(const char* file_path);
void ProcessInput(GLFWwindow* window);
void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void MouseCallback(GLFWwindow* window, double xposIn, double yposIn);
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

GLFWwindow* window { nullptr };
const int kScreenWidth  { 800 };
const int kScreenHeight { 600 };

Camera camera { glm::vec3 {0.0f, 1.5f, 6.0f} };
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
	unsigned int cube_texture  { LoadTexture("marble.png") };
	unsigned int plane_texture { LoadTexture("floor.jpg") };

	Vertex vertex;
	Shader shader {"framebuffers.vert", "framebuffers.frag"};
	Shader screen_shader{ "framebuffers_screen.vert", "framebuffers_screen.frag" };

	shader.use();
	shader.setInt("texture0", 0);    // set GL_TEXTURE0 to texture0 firstly
	screen_shader.use();
	screen_shader.setInt("screenTexture", 0);
	screen_shader.setVec2("screenSize", kScreenWidth, kScreenHeight);
	screen_shader.setFloat("pixelSize", 10.0f);

	// framebuffer configuration
	unsigned int framebuffer;
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	// create a color attachment texture
	unsigned int texture_colorbuffer;
	glGenTextures(1, &texture_colorbuffer);
	glBindTexture(GL_TEXTURE_2D, texture_colorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_colorbuffer, 0);
	// create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 512, 512); // use a single renderbuffer object for both a depth AND stencil buffer.
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
	// now that we actually created the framebuffer and added all attachments we want to check if it is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	while (!glfwWindowShouldClose(window)) {
		float current_frame = static_cast<float>(glfwGetTime());
		delta_time = current_frame - last_frame;
		last_frame = current_frame;

		ProcessInput(window);

		// before render to framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glEnable(GL_DEPTH_TEST);  // enable depth testing (is disabled for rendering screen-space quad)

		glClearColor(0.15f, 0.16f, 0.18f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // in the case we don't set stencil buffer

		// object
		shader.use();

		glm::mat4 model      { 1.0f };

		// make mirror texture
		glm::mat4 view       { camera.GetViewMatrix() };
		glm::mat4 projection { glm::perspective(glm::radians(camera.Zoom), (float)kScreenWidth / kScreenHeight, 0.1f, 100.0f) };
		shader.setMat4("model", model);
		shader.setMat4("view", view);
		shader.setMat4("projection", projection);

		// cubes
		glEnable(GL_CULL_FACE);
		glBindVertexArray(vertex.cubeVAO);
		glActiveTexture(GL_TEXTURE0);  // active GL_TEXTURE0 then bind texture
		glBindTexture(GL_TEXTURE_2D, cube_texture);
		model = glm::translate(model, glm::vec3(-2.0f, 0.0f, -2.0f));
		shader.setMat4("model", model);
		vertex.Draw(vertex.cubeVAO);  // first cube
		model = glm::mat4 { 1.0f };
		shader.setMat4("model", model);
		vertex.Draw(vertex.cubeVAO);  // second cube
		glDisable(GL_CULL_FACE);

		// plane
		glBindVertexArray(vertex.planeVAO);
		glBindTexture(GL_TEXTURE_2D, plane_texture);
		model = glm::mat4{ 1.0f };
		shader.setMat4("model", model);
		vertex.Draw(vertex.planeVAO);

		// now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// the same background color
		// clear all rexlevant buffers
		glClearColor(0.15f, 0.16f, 0.18f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Draw scene on normal screen
		model = glm::mat4 {1.0f};
		view = camera.GetViewMatrix();
		shader.setMat4("view", view);

		// cubes
		glEnable(GL_CULL_FACE);
		glBindVertexArray(vertex.cubeVAO);
		glActiveTexture(GL_TEXTURE0);  // active GL_TEXTURE0 then bind texture
		glBindTexture(GL_TEXTURE_2D, cube_texture);
		model = glm::translate(model, glm::vec3(-2.0f, 0.0f, -2.0f));
		shader.setMat4("model", model);
		vertex.Draw(vertex.cubeVAO);  // first cube
		model = glm::mat4{ 1.0f };
		shader.setMat4("model", model);
		vertex.Draw(vertex.cubeVAO);  // second cube
		glDisable(GL_CULL_FACE);

		// plane
		glBindVertexArray(vertex.planeVAO);
		glBindTexture(GL_TEXTURE_2D, plane_texture);
		model = glm::mat4{ 1.0f };
		shader.setMat4("model", model);
		vertex.Draw(vertex.planeVAO);

		//glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.

		screen_shader.use();
		model = glm::mat4 { 1.0f };
		view = camera.GetViewMatrix();
		screen_shader.setMat4("model", model);
		screen_shader.setMat4("view", view);
		screen_shader.setMat4("projection", projection);
		glBindVertexArray(vertex.screen_quadVAO);
		glBindTexture(GL_TEXTURE_2D, texture_colorbuffer);	// use the color attachment texture as the texture of the quad plane
		vertex.Draw(vertex.screen_quadVAO);
		glBindVertexArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	shader.Clean();
	screen_shader.Clean();
	vertex.Clean();
	glDeleteRenderbuffers(1, &rbo);
	glDeleteFramebuffers(1, &framebuffer);
}


int InitWindow() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	window = glfwCreateWindow(kScreenWidth, kScreenHeight, "Advanced: Frame Buffers", nullptr, nullptr);
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

unsigned int LoadTexture(const char* file_path) {
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(file_path, &width, &height, &nrComponents, 0);
	if (data) {
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
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

	if (glfwGetKey(window, GLFW_KEY_X))
		camera.ProcessKeyboard(kUpward, delta_time);
	if (glfwGetKey(window, GLFW_KEY_Z))
		camera.ProcessKeyboard(kDownward, delta_time);
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