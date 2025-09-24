#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Vertex {
public:
	Vertex();
	void Draw(unsigned int VAO);
	void Clean() const;

	unsigned int cubeVAO;
	unsigned int cubeVBO;
	unsigned int cubeEBO;
	unsigned int planeVAO;
	unsigned int planeVBO;
	unsigned int planeEBO;

private:
	void Init();
};
