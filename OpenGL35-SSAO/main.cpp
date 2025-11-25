#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <map>
#include <memory>
#include <random>
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
float lerp(GLfloat a, GLfloat b, GLfloat f);

GLFWwindow* window{ nullptr };
const int kScreenWidth{ 800 };
const int kScreenHeight{ 600 };

Camera camera{ glm::vec3 {0.0f, 0.0f, 5.0f} };
bool  first_mouse{ true };
float last_x{ static_cast<float>(kScreenWidth) / 2.0f };
float last_y{ static_cast<float>(kScreenHeight) / 2.0f };
float delta_time{ 0.0f };
float last_frame{ 0.0f };

bool equip_light { false };
bool is_e_pressed{ false };
bool ssao_enabled{ false };
bool is_space_pressed{ false };

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
	Shader shader_geometry_pass { "ssao_geometry.vert", "ssao_geometry.frag" };
	Shader shader_lighting_pass { "ssao.vert", "ssao_lighting.frag" };
	Shader shader_ssao { "ssao.vert", "ssao.frag" };
	Shader shader_light_box { "light_box.vert", "light_box.frag" };

	// Samplers
	shader_lighting_pass.use();
	shader_lighting_pass.setInt("gPositionDepth", 0);
	shader_lighting_pass.setInt("gNormal", 1);
	shader_lighting_pass.setInt("gAlbedoSpec", 2);
	shader_lighting_pass.setInt("ssao", 3);
	shader_ssao.use();
	shader_ssao.setInt("gPositionDepth", 0);
	shader_ssao.setInt("gNormal", 1);
	shader_ssao.setInt("texNoise", 2);

	// Objects
	Model object_model { "nanosuit_reflection/nanosuit.obj" };

	// Lights positions and colors
	glm::vec3 light_pos = glm::vec3(2.0, 4.0, -2.0);
	glm::vec3 light_color = glm::vec3(1.0f, 0.65f, 0.21f);

	// create g-buffer and config
	unsigned int gBuffer;                           // frame buffer
	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	unsigned int  gPositionDepth, gNormal, gAlbedoSpec;  // color attachments
	// 位置和法线需要更多精度
	// config position color attachment
	glGenTextures(1, &gPositionDepth);
	glBindTexture(GL_TEXTURE_2D, gPositionDepth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, kScreenWidth, kScreenHeight, 0, GL_RGBA, GL_FLOAT, nullptr);  // 使用GL_RGBA16F 可以存储默认范围 0.0 - 1.0 之外的浮点值
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPositionDepth, 0);
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

	// create framebuffer for SSAO pass
	unsigned int ssaoFBO;
	glGenFramebuffers(1, &ssaoFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
	unsigned int ssaoColorBuffer;
	glGenTextures(1, &ssaoColorBuffer);
	glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, kScreenWidth, kScreenHeight, 0, GL_RED, GL_FLOAT, nullptr);  // we only need a single color value for SSAO
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBuffer, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Framebuffer 2 not complete!" << std::endl;
	}

	// SSAO sample kernel
	std::uniform_real_distribution<float> random_floats{ 0.0f, 1.0f }; // random floats between 0.0 - 1.0
	std::default_random_engine generator;
	std::vector<glm::vec3> ssao_kernel;
	ssao_kernel.reserve(64);
	for (int i = 0; i < 64; ++i) {
		// random distributed samplers in x[-1,1] y[-1,1] z[0,1] then nomalized, so we can get an half unit ball
		// and assume it is in tangent space (xy is UV)
		glm::vec3 sample{
			random_floats(generator) * 2.0f - 1.0f,
			random_floats(generator) * 2.0f - 1.0f,
			random_floats(generator)
		};
		sample = glm::normalize(sample);  // 单位化到球面上

		// 这里再随机缩放，是为了在球体内均匀分布，而不是在球面上均匀分布
		sample *= random_floats(generator);

		// 加速插值使采样点在中心更密集，边缘稀疏（二次函数是曲线x:[1/64, 63/64], y:[0.1, 1.0-]）
		// Scale samples s.t. they're more aligned to center of kernel
		float scale { static_cast<float>(i) / 64.0f };
		scale = lerp(0.1f, 1.0f, scale * scale);
		sample *= scale;
		ssao_kernel.emplace_back(sample);
	}

	// ssao noise texture
	std::vector<glm::vec3> ssaoNoise;
	ssaoNoise.reserve(16);
	for (int i = 0; i < 16; ++i) {
		// rotation around z-axis only
		// 实际上并不是旋转，而是在xy平面上的随机偏移，这样在切线空间就实现了随机旋转一定角度的效果
		glm::vec3 noise{
			random_floats(generator) * 2.0f - 1.0f,
			random_floats(generator) * 2.0f - 1.0f,
			0.0f
		};
		ssaoNoise.emplace_back(noise);
	}
	unsigned int noiseTexture;
	glGenTextures(1, &noiseTexture);
	glBindTexture(GL_TEXTURE_2D, noiseTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 4, 4, 0, GL_RED, GL_FLOAT, &ssaoNoise[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	while (!glfwWindowShouldClose(window)) {
		float current_frame = static_cast<float>(glfwGetTime());
		delta_time = current_frame - last_frame;
		last_frame = current_frame;

		ProcessInput(window);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (equip_light) {
			glm::mat3 camera_rotation = glm::mat3{ camera.Right, camera.Up, -camera.Front };
			light_pos = camera.Position + camera_rotation * glm::vec3{ 0.0f, 0.0f, -2.0f };
		}

		glm::mat4 model{ 1.0f };
		glm::mat4 view{ camera.GetViewMatrix() };
		glm::mat4 projection{ glm::perspective(glm::radians(camera.Zoom), static_cast<float>(kScreenWidth) / static_cast<float>(kScreenHeight), 0.1f, 50.0f) };
		
		// geometry pass
		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader_geometry_pass.use();
		shader_geometry_pass.setMat4("view", view);
		shader_geometry_pass.setMat4("projection", projection);
		model = glm::translate(model, glm::vec3(0.0, -1.5f, 0.0f));
		model = glm::scale(model, glm::vec3(20.0f, 1.0f, 20.0f));
		shader_geometry_pass.setMat4("model", model);
		glBindVertexArray(vertex.cubeVAO);
		vertex.Draw(vertex.cubeVAO);

		model = glm::mat4{ 1.0f };
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 5.0));
		model = glm::rotate(model, glm::radians(- 90.0f), glm::vec3(1.0, 0.0, 0.0));
		model = glm::scale(model, glm::vec3(0.5f));
		shader_geometry_pass.setMat4("model", model);
		object_model.Draw(shader_geometry_pass);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		
		// SSAO pass
		glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shader_ssao.use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gPositionDepth);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gNormal);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, noiseTexture);
		shader_ssao.setMat4("projection", projection);
		for (unsigned int i = 0; i < 64; ++i) {
			shader_ssao.setVec3("samples[" + std::to_string(i) + "]", ssao_kernel[i]);
		}
		glBindVertexArray(vertex.quadVAO);
		vertex.Draw(vertex.quadVAO);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// lighting pass
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shader_lighting_pass.use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gPositionDepth);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gNormal);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
		float linear = 0.09f;
		float quadratic = 0.032f;
		view = camera.GetViewMatrix();
		glm::vec3 light_view_pos{ glm::vec3{ view * glm::vec4 { light_pos, 1.0f } } };
		shader_lighting_pass.setVec3("light.Position", light_view_pos);
		shader_lighting_pass.setVec3("light.Color", light_color);
		shader_lighting_pass.setFloat("light.Linear", linear);
		shader_lighting_pass.setFloat("light.Quadratic", quadratic);
		shader_lighting_pass.setBool("ssao_enabled", ssao_enabled);

		glBindVertexArray(vertex.quadVAO);
		vertex.Draw(vertex.quadVAO);

		// now we copy depth info to default framebuffer's depth buffer
		glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer
		glBlitFramebuffer(0, 0, kScreenWidth, kScreenHeight, 0, 0, kScreenWidth, kScreenHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// light box
		shader_light_box.use();
		shader_light_box.setMat4("view", view);
		shader_light_box.setMat4("projection", projection);
		shader_light_box.setVec3("lightColor", light_color);
		model = glm::mat4{ 1.0f };
		model = glm::translate(model, light_pos);
		model = glm::scale(model, glm::vec3(0.2f));
		shader_light_box.setMat4("model", model);

		glBindVertexArray(vertex.ballVAO);
		vertex.Draw(vertex.ballVAO);

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

	if (!is_e_pressed && glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		equip_light = !equip_light;
		is_e_pressed = true;
	}

	if (is_e_pressed && glfwGetKey(window, GLFW_KEY_E) == GLFW_RELEASE) {
		is_e_pressed = false;
	}

	if (!is_space_pressed && glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		ssao_enabled = !ssao_enabled;
		is_space_pressed = true;
	}

	if (is_space_pressed && glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {
		is_space_pressed = false;
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

float lerp(GLfloat a, GLfloat b, GLfloat f) {
	return a + f * (b - a);
}
