#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

using VAOType = unsigned int;

class Vertex {
public:
	Vertex();
	void Draw(unsigned int VAO);
	void Clean() const;

	VAOType cubeVAO;
	unsigned int cubeVBO;
	unsigned int cubeEBO;
	VAOType planeVAO;
	unsigned int planeVBO;
	unsigned int planeEBO;
	VAOType vegatationVAO;
	unsigned int vegatationVBO;
	unsigned int vegatationEBO;
	VAOType quadVAO;
	unsigned int quadVBO;
	unsigned int quadEBO;
	VAOType screen_quadVAO;
	unsigned int screen_quadVBO;
	unsigned int screen_quadEBO;

private:
	void Init();
};
