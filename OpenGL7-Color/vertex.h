#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Vertex {
public:
	Vertex();
	void Clean() const;

private:
	unsigned int VAO_, VBO_, EBO_;
};
