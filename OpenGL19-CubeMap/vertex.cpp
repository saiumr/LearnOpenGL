#include "vertex.h"
#include <functional>

float cube_vertices[] = {
	// 确保从同一侧观察box时，相对的两个面，正面顶点顺序是逆时针，背面顶点顺序是顺时针即可
	// 环绕观察定义顶点时，都按照逆时针顺序定义顶点
    // position        // texture coord (S,T,[R])  // normal vector
    // bottom face (eyes around box)
    -0.5f,-0.5f,-0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, // bottom-left
     0.5f,-0.5f,-0.5f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, // bottom-right
     0.5f,-0.5f, 0.5f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f, // top-right
    -0.5f,-0.5f, 0.5f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f, // top-left

    // back face
     0.5f,-0.5f,-0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, // bottom-left
    -0.5f,-0.5f,-0.5f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, // bottom-right
    -0.5f, 0.5f,-0.5f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f, // top-right
     0.5f, 0.5f,-0.5f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f, // top-left

    // left face
    -0.5f,-0.5f,-0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
    -0.5f,-0.5f, 0.5f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-right
    -0.5f, 0.5f, 0.5f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, // top-right
    -0.5f, 0.5f,-0.5f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, // top-left

    // right face
     0.5f,-0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
     0.5f,-0.5f,-0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-right
     0.5f, 0.5f,-0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, // top-right
     0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, // top-left

     // top face
    -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom-left
     0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom-right
     0.5f, 0.5f,-0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, // top-right
    -0.5f, 0.5f,-0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // top-left

     // front face
    -0.5f,-0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
     0.5f,-0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
     0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-right
    -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
};

// a cube
// image your eyes move around box and staring straight at every face, ensure that each triangle is drawn in a counterclockwise order
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

Vertex::Vertex() {
    Init();
}

void Vertex::Clean() const {
	glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteBuffers(1, &cubeEBO);
}

void Vertex::Init() {
    // Gen VAO VBO EBO
	glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glGenBuffers(1, &cubeEBO);

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
    
	// unbind VAO VBO EBO
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);  // EBO must unbind after VAO unbind
}

void Vertex::Draw(VAOType VAO) {
    if (VAO == cubeVAO) {
        glDrawElements(GL_TRIANGLES, sizeof(cube_indices)/sizeof(unsigned int), GL_UNSIGNED_INT, 0);
    }
}
