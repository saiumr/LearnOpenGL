#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <map>
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

Camera camera { glm::vec3 {0.0f, 1.0f, 3.0f} };
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
	Shader shader_red    { "advanced_glsl.vert", "red.frag"    };
	Shader shader_green  { "advanced_glsl.vert", "green.frag"  };
	Shader shader_blue   { "advanced_glsl.vert", "blue.frag"   };
	Shader shader_yellow { "advanced_glsl.vert", "yellow.frag" };

	// uniform has max size, we can check it by GL_MAX_VERTEX_UNIFORM_COMPONENTS
	// so use uniform buffer object(UBO) to store uniforms we can use store uniforms
	// we can always use UBO when making bones animation
	std::cout << "max uniform: " << GL_MAX_VERTEX_UNIFORM_COMPONENTS << std::endl;

	// first. we get the uniform block index
	unsigned int uniform_block_index_red = glGetUniformBlockIndex(shader_red.ID, "Matrices");
	unsigned int uniform_block_index_green = glGetUniformBlockIndex(shader_green.ID, "Matrices");
	unsigned int uniform_block_index_blue = glGetUniformBlockIndex(shader_blue.ID, "Matrices");
	unsigned int uniform_block_index_yellow = glGetUniformBlockIndex(shader_yellow.ID, "Matrices");

	// then we link each shader's uniform block to this uniform binding point
	glUniformBlockBinding(shader_red.ID, uniform_block_index_red, 0);
	glUniformBlockBinding(shader_green.ID, uniform_block_index_green, 0);
	glUniformBlockBinding(shader_blue.ID, uniform_block_index_blue, 0);
	glUniformBlockBinding(shader_yellow.ID, uniform_block_index_yellow, 0);

	// Now actually create the buffer
	unsigned int ubo_matrices;
	glGenBuffers(1, &ubo_matrices);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo_matrices);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), nullptr, GL_STATIC_DRAW); // allocate 2 mat4s
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	// define the range of the buffer that links to a uniform binding point
	// bind 0~2*sizeof(mat4) bytes of ubo_matrices to binding point 0
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, ubo_matrices, 0, 2 * sizeof(glm::mat4)); // we bind the buffer to the binding point 0

	// store the projection matrix (we only have to do this once)(note: we're not using zoom anymore by changing the FoV)
	glm::mat4 projection = glm::perspective(45.0f, (float)kScreenWidth / kScreenHeight, 0.1f, 100.0f);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo_matrices);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection)); // note that we use glBufferSubData, not glBufferData
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	while (!glfwWindowShouldClose(window)) {
		float current_frame = static_cast<float>(glfwGetTime());
		delta_time = current_frame - last_frame;
		last_frame = current_frame;

		ProcessInput(window);

		glClearColor(0.15f, 0.16f, 0.18f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// set view and projection matrix in the block - we only have to do this once per loop iteration.
		glm::mat4 view { camera.GetViewMatrix() };
		glBindBuffer(GL_UNIFORM_BUFFER, ubo_matrices);
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view)); // note that we use glBufferSubData, not glBufferData
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		// draw 4 cubes 
		// RED
		glBindVertexArray(vertex.cubeVAO);
		shader_red.use();
		glm::mat4 model { glm::mat4(1.0f) };
		model = glm::translate(model, glm::vec3(-0.75f, 0.75f, 0.0f)); // move top-left
		shader_red.setMat4("model", model);
		vertex.Draw(vertex.cubeVAO);
		// GREEN
		shader_green.use();
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.75f, 0.75f, 0.0f)); // move top-right
		shader_green.setMat4("model", model);
		vertex.Draw(vertex.cubeVAO);
		// YELLOW
		shader_yellow.use();
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-0.75f, -0.75f, 0.0f)); // move bottom-left
		shader_yellow.setMat4("model", model);
		vertex.Draw(vertex.cubeVAO);
		// BLUE
		shader_blue.use();
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.75f, -0.75f, 0.0f)); // move bottom-right
		shader_blue.setMat4("model", model);
		vertex.Draw(vertex.cubeVAO);

		glBindVertexArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	shader_red.Clean();
	shader_green.Clean();
	shader_blue.Clean();
	shader_yellow.Clean();
	vertex.Clean();
}


int InitWindow() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	window = glfwCreateWindow(kScreenWidth, kScreenHeight, "Advanced: GLSL(UBO)", nullptr, nullptr);
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
	// enable following line to allow us to modify point size by set gl_PointSize in vertex shader
	//glEnable(GL_PROGRAM_POINT_SIZE);

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
		GLenum format { GL_RGB };
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
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