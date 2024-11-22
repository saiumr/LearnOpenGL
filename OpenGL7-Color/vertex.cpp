#include "vertex.h"

float vertices[] = {
    // position         // color          // texture coord (S,T,[R])  // normal vector
    // bottom face (image your eyes in box and staring straight at a face)
    -0.5f,-0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, // bottom-left
     0.5f,-0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, // bottom-right
     0.5f,-0.5f,-0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f, // top-right
    -0.5f,-0.5f,-0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f, // top-left

    // back face
    -0.5f,-0.5f,-0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, // bottom-left
     0.5f,-0.5f,-0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, // bottom-right
     0.5f, 0.5f,-0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f, // top-right
    -0.5f, 0.5f,-0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f, // top-left

    // left face
    -0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
    -0.5f,-0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-right
    -0.5f,-0.5f,-0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, // top-right
    -0.5f, 0.5f,-0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, // top-left

    // right face
     0.5f,-0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
     0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-right
     0.5f, 0.5f,-0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, // top-right
     0.5f,-0.5f,-0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, // top-left

     // top face
     0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom-left
    -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom-right
    -0.5f, 0.5f,-0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, // top-right
     0.5f, 0.5f,-0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // top-left

     // front face
    -0.5f,-0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
     0.5f,-0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
     0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-right
    -0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
};

// a cube
unsigned int indeices[] = {
    // bottom face
    0, 1, 2,
    2, 3, 0,

    // bace face
    4, 5, 6,
    6, 7, 4,

    // left face
    8, 9, 10,
    10, 11, 8,

    // right face
    12, 13, 14,
    14, 15, 12,

    // top face
    16, 17, 18,
    18, 19, 16,

    // front face
    20, 21, 22,
    22, 23, 20,
};

Vertex::Vertex() {
    Init();
}

void Vertex::Clean() const {
	glDeleteVertexArrays(1, &this->VAO_);
	glDeleteBuffers(1, &this->VBO_);
	glDeleteBuffers(1, &this->EBO_);
}

unsigned int Vertex::get_VAO() const {
	return VAO_;
}
unsigned int Vertex::get_LightVAO() const {
    return LightVAO_;
}
unsigned int Vertex::get_VBO() const {
	return VBO_;
}
unsigned int Vertex::get_EBO() const {
	return EBO_;
}

unsigned int Vertex::get_ElementCount() const {
    return sizeof(indeices) / sizeof(unsigned int);
}

void Vertex::Init() {
	glGenVertexArrays(1, &VAO_);
    glGenVertexArrays(1, &LightVAO_);
    glGenBuffers(1, &VBO_);
    glGenBuffers(1, &EBO_);

    glBindVertexArray(VAO_);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indeices), indeices, GL_STATIC_DRAW);

    // tell OpenGL how to interpret vertex data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GL_FLOAT), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GL_FLOAT), (void*)(6 * sizeof(GL_FLOAT)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    // bind second VAO
    glBindVertexArray(LightVAO_);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_);  // VBO_ and EBO_ has included buffer data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GL_FLOAT), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
