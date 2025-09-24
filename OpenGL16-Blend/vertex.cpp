#include "vertex.h"
#include <functional>

float cube_vertices[] = {
    // position        // texture coord (S,T,[R])  // normal vector
    // bottom face (image your eyes in box and staring straight at a face)
    -0.5f,-0.5f, 0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, // bottom-left
     0.5f,-0.5f, 0.5f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, // bottom-right
     0.5f,-0.5f,-0.5f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f, // top-right
    -0.5f,-0.5f,-0.5f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f, // top-left

    // back face
    -0.5f,-0.5f,-0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, // bottom-left
     0.5f,-0.5f,-0.5f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, // bottom-right
     0.5f, 0.5f,-0.5f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f, // top-right
    -0.5f, 0.5f,-0.5f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f, // top-left

    // left face
    -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
    -0.5f,-0.5f, 0.5f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-right
    -0.5f,-0.5f,-0.5f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, // top-right
    -0.5f, 0.5f,-0.5f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, // top-left

    // right face
     0.5f,-0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
     0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-right
     0.5f, 0.5f,-0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, // top-right
     0.5f,-0.5f,-0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, // top-left

     // top face
     0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom-left
    -0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom-right
    -0.5f, 0.5f,-0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, // top-right
     0.5f, 0.5f,-0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // top-left

     // front face
    -0.5f,-0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
     0.5f,-0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
     0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-right
    -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
};

// a cube
unsigned int cube_indices[] = {
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

float plane_vertices[] = {
    // positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
     5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
    -5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
    -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

     5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
    -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
     5.0f, -0.5f, -5.0f,  2.0f, 2.0f
};

unsigned int plane_indices[] = {
    0, 1, 2,
    3, 4, 5
};

Vertex::Vertex() {
    Init();
}

void Vertex::Clean() const {
	glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteBuffers(1, &cubeEBO);
	glDeleteVertexArrays(1, &planeVAO);
    glDeleteBuffers(1, &planeVBO);
    glDeleteBuffers(1, &planeEBO);
}

void Vertex::Init() {
	glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glGenBuffers(1, &cubeEBO);
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glGenBuffers(1, &planeEBO);

	// bind first VAO
    glBindVertexArray(cubeVAO);

    // bind and set VBO
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

	// set vertex attribute pointers to VAO
    glEnableVertexAttribArray(0);  // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (void*)0);
	glEnableVertexAttribArray(1);  // texture coord
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
	//glEnableVertexAttribArray(2);  // normal vector
    //glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GL_FLOAT), (void*)(6 * sizeof(GL_FLOAT)));
    
	// bind and set EBO
	// EBO must bind after VAO bind
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);

	// unbind VAO, VBO, EBO
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    // set second VAO
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);  // VBO_ and EBO_ has included buffer data
	glBufferData(GL_ARRAY_BUFFER, sizeof(plane_vertices), plane_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (void*)0);
    glEnableVertexAttribArray(1);  // texture coord
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planeEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(plane_indices), plane_indices, GL_STATIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);  // EBO must unbind after VAO unbind
}

void Vertex::Draw(unsigned int VAO) {
    if (VAO == cubeVAO) {
        glDrawElements(GL_TRIANGLES, sizeof(cube_indices)/sizeof(unsigned int), GL_UNSIGNED_INT, 0);
    } else if (VAO == planeVAO) {
        glDrawElements(GL_TRIANGLES, sizeof(plane_indices)/sizeof(unsigned int), GL_UNSIGNED_INT, 0);
    }
}
