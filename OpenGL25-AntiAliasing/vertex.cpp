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

float vegatation_vertices[] = {
    // positions          // texture Coords (swapped y coordinates because texture is flipped upside down)
     0.0f, -0.5f,  0.5f,  0.0f, 1.0f,
     0.0f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.0f,  0.5f,  0.5f,  0.0f, 0.0f,

     0.0f,  0.5f, -0.5f,  1.0f, 0.0f,
     0.0f,  0.5f,  0.5f,  0.0f, 0.0f,
     0.0f, -0.5f, -0.5f,  1.0f, 1.0f
};

unsigned int vegatation_indices[] = {
    0, 1, 2,
    3, 4, 5
};

float points_vertices[] = {
	// x,y        // r,g,b
    -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, // top-left
     0.5f,  0.5f, 0.0f, 1.0f, 0.0f, // top-right
     0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // bottom-right
    -0.5f, -0.5f, 1.0f, 1.0f, 0.0f  // bottom-left
};

unsigned int points_indices[] = {
    0, 1, 2,
    2, 3, 0
};

float rect_vertices[] = {
    // x,y        // r,g,b
   -0.05f,  0.05f, 1.0f, 0.0f, 0.0f,
   -0.05f, -0.05f, 0.0f, 1.0f, 0.0f,
    0.05f, -0.05f, 0.0f, 0.0f, 1.0f,
    0.05f,  0.05f, 1.0f, 1.0f, 0.0f
};

unsigned int rect_indices[] = {
    0, 1, 2,
    2, 3, 0
};

float quad_vertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
    // positions        // texCoords
    -1.0f,  1.0f, 0.0f, 1.0f,
    -1.0f, -1.0f, 0.0f, 0.0f,
     1.0f, -1.0f, 1.0f, 0.0f,
     1.0f,  1.0f, 1.0f, 1.0f
};

unsigned int quad_indices[] = {
    0, 1, 2,
    2, 3, 0
};

Vertex::Vertex() {
    Init();
}

void Vertex::Clean() const {
	glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteBuffers(1, &cubeEBO);
	glDeleteVertexArrays(1, &vegatationVAO);
	glDeleteBuffers(1, &vegatationVBO);
    glDeleteBuffers(1, &vegatationEBO);
	glDeleteVertexArrays(1, &planeVAO);
    glDeleteBuffers(1, &planeVBO);
    glDeleteBuffers(1, &planeEBO);
	glDeleteVertexArrays(1, &pointVAO);
    glDeleteBuffers(1, &pointVBO);
	glDeleteBuffers(1, &pointEBO);
	glDeleteVertexArrays(1, &rectVAO);
    glDeleteBuffers(1, &rectVBO);
	glDeleteBuffers(1, &rectEBO);
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
    glDeleteBuffers(1, &quadEBO);
}

void Vertex::Init() {
    // Gen VAO VBO EBO
	glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glGenBuffers(1, &cubeEBO);
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glGenBuffers(1, &planeEBO);
    glGenVertexArrays(1, &vegatationVAO);
    glGenBuffers(1, &vegatationVBO);
    glGenBuffers(1, &vegatationEBO);
	glGenVertexArrays(1, &pointVAO);
    glGenBuffers(1, &pointVBO);
	glGenBuffers(1, &pointEBO);
	glGenVertexArrays(1, &rectVAO);
	glGenBuffers(1, &rectVBO);
	glGenBuffers(1, &rectEBO);
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
	glGenBuffers(1, &quadEBO);

	// bind first VAO
    glBindVertexArray(cubeVAO);

    // bind and set VBO
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

	// set vertex attribute pointers to VAO
    glEnableVertexAttribArray(0);  // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (void*)0);
	//glEnableVertexAttribArray(1);  // texture coord
    //glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
	//glEnableVertexAttribArray(2);  // normal vector
    //glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GL_FLOAT), (void*)(6 * sizeof(GL_FLOAT)));
    
	// bind and set EBO
	// EBO must bind after VAO bind
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);
    
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

	// vegetation VAO
	glBindVertexArray(vegatationVAO);
	glBindBuffer(GL_ARRAY_BUFFER, vegatationVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vegatation_vertices), vegatation_vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);  // position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (void*)0);
    glEnableVertexAttribArray(1);  // texture coord
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vegatationEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vegatation_indices), vegatation_indices, GL_STATIC_DRAW);

	// point VAO
	glBindVertexArray(pointVAO);
	glBindBuffer(GL_ARRAY_BUFFER, pointVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points_vertices), points_vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);  // position
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (void*)0);
	glEnableVertexAttribArray(1);  // r,g,b
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (void*)(2 * sizeof(GL_FLOAT)));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pointEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(points_indices), points_indices, GL_STATIC_DRAW);

	// rect VAO
	glBindVertexArray(rectVAO);
	glBindBuffer(GL_ARRAY_BUFFER, rectVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rect_vertices), rect_vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);  // position
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (void*)0);
	glEnableVertexAttribArray(1);  // r,g,b
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (void*)(2 * sizeof(GL_FLOAT)));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rectEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(rect_indices), rect_indices, GL_STATIC_DRAW);

	// quad VAO
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);  // position
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GL_FLOAT), (void*)0);
    glEnableVertexAttribArray(1);  // texture coord
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GL_FLOAT), (void*)(2 * sizeof(GL_FLOAT)));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quad_indices), quad_indices, GL_STATIC_DRAW);

	// unbind VAO VBO EBO
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);  // EBO must unbind after VAO unbind
}

void Vertex::Draw(VAOType VAO) {
    if (VAO == cubeVAO) {
        glDrawElements(GL_TRIANGLES, sizeof(cube_indices)/sizeof(unsigned int), GL_UNSIGNED_INT, 0);
    } else if (VAO == planeVAO) {
        glDrawElements(GL_TRIANGLES, sizeof(plane_indices)/sizeof(unsigned int), GL_UNSIGNED_INT, 0);
    } else if (VAO == vegatationVAO) {
		glDrawElements(GL_TRIANGLES, sizeof(vegatation_indices) / sizeof(unsigned int), GL_UNSIGNED_INT, 0);
	} else if (VAO == pointVAO) {
        glDrawElements(GL_POINTS, sizeof(points_vertices) / sizeof(unsigned int), GL_UNSIGNED_INT, 0);
    } else if (VAO == rectVAO) {
		// 100 instances
		glDrawElementsInstanced(GL_TRIANGLES, sizeof(rect_indices) / sizeof(unsigned int), GL_UNSIGNED_INT, 0, 100);
    } else if (VAO == quadVAO) {
        glDrawElements(GL_TRIANGLES, sizeof(quad_indices) / sizeof(unsigned int), GL_UNSIGNED_INT, 0);
    }
}
