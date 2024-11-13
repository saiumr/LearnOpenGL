#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include "shader.h"
#include "texture.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// Init
int initWindow();

// Render
void renderLoop(Shader& shader);

// base data
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;
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
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL3: Texture", NULL, NULL);
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
    // default

    float vertices[] = {
        // position         // color          // texture coord (S,T,[R])
        -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // left bottom 
         0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // right bottom
         0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // right top
        -0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f  // left top
    };


    // practice 2: 4 face
    /*
    float vertices[] = {
        // position         // color          // texture coord (S,T,[R])
        -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // left bottom 
         0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 2.0f, 0.0f, // right bottom
         0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 2.0f, 2.0f, // right top
        -0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 2.0f  // left top
    };
    */
    

    // practice 3: a part of picture
    /*
    float vertices[] = {
        // positions          // colors           // texture coords (note that we changed them to 'zoom in' on our texture image)
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   0.55f, 0.55f, // top right
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   0.55f, 0.45f, // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.45f, 0.45f, // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.45f, 0.55f  // top left 
    };
    */

    unsigned int indeices[] = {
        0, 1, 2,
        2, 3, 0,
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
    glEnableVertexAttribArray(1);
    // texture coordinate
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (void*)(6 * sizeof(GL_FLOAT)));
    glEnableVertexAttribArray(2);
    // set texture to uniform sampler ourTexture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, createTexture("container.jpg"));
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, createTexture("awesomeface.png"));

    // After setting data, unbind objs (use function the same as bind)
    glBindBuffer(GL_ARRAY_BUFFER, 0);           // VBO 0
    glBindVertexArray(0);                       // VAO 0
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);   // EBO 0

    // Draw mode
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  // GL_FILL

    // active shader
    shader.use();
    // associate the shader with the texture unit
    glUniform1i(glGetUniformLocation(shader.ID, "texture0"), 0);
    shader.setInt("texture1", 1);
    // set position
    shader.setFloat("movePosition", 0.0f);

    float blend = 0.33f;
    // Render loop
    while (!glfwWindowShouldClose(window)) {

        // Process input
        processInput(window);
        // render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // practice 4: set blend value (no fps control so that change so fast)
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS ) {
            blend = blend + 0.001f;
            if (blend >= 1.0f) {
                blend = 1.0f;
            }
        }
        else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            blend = blend - 0.001f;
            if (blend <= 0.0f) {
                blend = 0.0f;
            }
        }
        shader.setFloat("blend", blend);

        // first translate then ratate(in while) by the time.
        // Remember that matrix multiplication is applied in reverse.
        glm::mat4 trans;
        trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
        trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
        shader.setMat4("transform", trans);
        
        // draw triangle
        glBindVertexArray(VAO);  // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        //glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        // glBindVertexArray(0); // no need to unbind it every time 

        // better transform order: scale, rotate, translate
        trans = glm::mat4{ 1.0f };
        trans = glm::translate(trans, glm::vec3(-0.5f, 0.5f, 0.0f));
        trans = glm::scale(trans, glm::vec3(std::sin((float)glfwGetTime()), std::sin((float)glfwGetTime()), std::sin((float)glfwGetTime())));
        shader.setMat4("transform", trans);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

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
