#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include "shader.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// Init
int initWindow();

// Render
void renderLoop(Shader& shader);

// base data
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
GLFWwindow* window = nullptr;


int main()
{
    if (initWindow() < 0) {
        return -1;
    }

    Shader shader{"shader.vs", "shader.fs"};
    renderLoop(shader);

    return 0;
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}



int initWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Create GLFW window
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL1: Draw a triangle", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Init GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to init GLAD" << std::endl;
        return -1;
    }

    // Set viewport
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    return 0;
}

void renderLoop(Shader& shader) {
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        // position         // color
        -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // left bottom 
         0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // right bottom
         0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f, // mid top for triangle
        //  0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f, // right top
        // -0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 0.0f, // left top
    };

    unsigned int indeices[] = {
        0, 1, 2,
        //2, 3, 0,
    };

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind VAO first
    glBindVertexArray(VAO);
    // bind VBO and set data
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // bind EBO and set data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indeices), indeices, GL_STATIC_DRAW);
    // Define how to read vertex attribute data from client memory (usually from buffer objects) and pass it to the vertex shader.
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
    glEnableVertexAttribArray(1);
    // After setting data, unbind objs (use function the same as bind)
    glBindBuffer(GL_ARRAY_BUFFER, 0);           // VBO 0
    glBindVertexArray(0);                       // VAO 0
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);   // EBO 0

    // Draw mode
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  // GL_FILL

    // Render loop
    while (!glfwWindowShouldClose(window)) {

        // Process input
        processInput(window);

        // render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);


        // active shader
        shader.use();
        // set position
        shader.setFloat("movePosition", 0.3f);
        
        // draw triangle
        glBindVertexArray(VAO);  // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        //glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        // glBindVertexArray(0); // no need to unbind it every time 

        // Swap buffer and poll IO events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // clean 
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shader.ID);

    // Clean all assets
    glfwTerminate();
}
