#include "vertex.h"

void Vertex::Clean() const {
	glDeleteVertexArrays(1, &this->VAO_);
	glDeleteBuffers(1, &this->VBO_);
	glDeleteBuffers(1, &this->EBO_);
}
