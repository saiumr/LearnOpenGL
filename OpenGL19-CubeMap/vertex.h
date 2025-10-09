#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

using VAOType = unsigned int;

class Vertex {
public:
	Vertex();
	void Draw(VAOType VAO);
	void Clean() const;

	VAOType cubeVAO;
	unsigned int cubeVBO;
	unsigned int cubeEBO;
	VAOType skyboxVAO;
	unsigned int skyboxVBO;
	unsigned int skyboxEBO;

private:
	void Init();
};
