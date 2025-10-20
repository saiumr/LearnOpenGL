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
	VAOType pointVAO;
	unsigned int pointVBO;
	unsigned int pointEBO;
	VAOType rectVAO;
	unsigned int rectVBO;
	unsigned int rectEBO;

private:
	void Init();
};
