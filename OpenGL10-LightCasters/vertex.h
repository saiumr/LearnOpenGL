#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Vertex {
public:
	Vertex();
	void Clean() const;
	unsigned int get_VAO() const;
	unsigned int get_LightVAO() const;
	unsigned int get_VBO() const;
	unsigned int get_EBO() const;
	unsigned int get_ElementCount() const;

private:
	void Init();
	unsigned int VAO_, VBO_, EBO_;
	unsigned int LightVAO_;
};
