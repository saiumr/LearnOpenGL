#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <vector>

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
	VAOType quadVAO;
	unsigned int quadVBO;
	unsigned int quadEBO;
	VAOType ballVAO;
	unsigned int ballVBO;
	unsigned int ballEBO;

private:
	void Init();
	void GenBallVertices(float radius = 1.0f, int slices = 16, int stacks = 16);
	std::vector<float> ball_vertices_;
	std::vector<unsigned int> ball_indices_;
};
