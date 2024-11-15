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
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL6: Camera", NULL, NULL);
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

    // enable depth test
    glEnable(GL_DEPTH_TEST);

    return 0;
}

void renderLoop(Shader& shader) {
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    // default
    float vertices[] = {
        // position         // color          // texture coord (S,T,[R])
        // bottom face (image your eyes in box and staring straight at a face)
        -0.5f,-0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom-left
         0.5f,-0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
         0.5f,-0.5f,-0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // top-right
        -0.5f,-0.5f,-0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left

        // back face
        -0.5f,-0.5f,-0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom-left
         0.5f,-0.5f,-0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
         0.5f, 0.5f,-0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // top-right
        -0.5f, 0.5f,-0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left

        // left face
        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom-left
        -0.5f,-0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
        -0.5f,-0.5f,-0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // top-right
        -0.5f, 0.5f,-0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left

        // right face
         0.5f,-0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom-left
         0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
         0.5f, 0.5f,-0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // top-right
         0.5f,-0.5f,-0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left

         // top face
         0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom-left
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
        -0.5f, 0.5f,-0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // top-right
         0.5f, 0.5f,-0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left

        // front face
        -0.5f,-0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom-left
         0.5f,-0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
         0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // top-right
        -0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left

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

    // more cubes
    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f,  0.0f,  0.0f),
        glm::vec3(2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f,  2.0f, -2.5f),
        glm::vec3(1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
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
    glBindTexture(GL_TEXTURE_2D, createTexture("yin_tiger.png"));

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

    // OpenGL5: Coordinate System
    glm::mat4 model, view, projection;
    //model      = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    view       = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f)); // Translate the entire scene backward by 3 units.
    projection = glm::perspective(glm::radians(45.0f), static_cast<float>(SCR_WIDTH) / SCR_HEIGHT, 0.1f, 100.0f);
    shader.setMat4("model", model);  // glm 0.9.8, model is identity matrix
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);

    // OpenGL6: Camera
    glm::vec3 cameraPos   = glm::vec3{ 0.0f, 0.0f, 3.0f };
    glm::vec3 cameraFront = glm::vec3{ 0.0f, 0.0f, -1.0f }; // the vector of camera position substract target position
    glm::vec3 cameraUp    = glm::vec3{ 0.0f, 1.0f, 0.0f };
    float deltaTime = 0.0f;    // unified moving speed
    float lastTime = 0.0f;

    float blend = 0.50f;
    // Render loop
    while (!glfwWindowShouldClose(window)) {
        float currentTime = static_cast<float>(glfwGetTime());
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        // Process input
        processInput(window);
        // render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
        
        // OpenGL6: Camera
        view = glm::mat4{ 1.0f };
        float cameraSpeed = static_cast<float>(1.0f * deltaTime);
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            cameraPos += cameraSpeed * cameraFront;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            cameraPos -= cameraSpeed * cameraFront;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        }
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        shader.setMat4("view", view);

        // draw triangle
        glBindVertexArray(VAO);  // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized

        for (unsigned int i = 0; i < 10; i++)
        {
            model = glm::mat4{1.0f};
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * (i+1) * (float)glfwGetTime();
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            shader.setMat4("model", model);

            glDrawElements(GL_TRIANGLES, 6 * 6, GL_UNSIGNED_INT, 0);
        }

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
