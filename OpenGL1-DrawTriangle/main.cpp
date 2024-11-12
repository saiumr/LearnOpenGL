#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// Compile shader
unsigned int compileShader(const char* shaderSource, GLenum shaderType);

// Link shader
unsigned int linkShader(const std::vector<unsigned int>& shaders);

// Use shader program
void useShader();

// Draw triangle
void drawTriangle();

// Init
int initWindow();

// Render
void renderLoop();

// base data
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
GLFWwindow* window = nullptr;

// shader data
const char* vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

const char* fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "    FragColor = vec4(0.44f, 0.38f, 0.91f, 1.0f);\n"
    "}\0";

int main()
{
    if (initWindow() < 0) {
        return -1;
    }

    renderLoop();

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

unsigned int compileShader(const char* shaderSource, GLenum shaderType) {
    int          success;
    char         errorLog[512];
    unsigned int Shader;
    
    Shader = glCreateShader(shaderType);
    glShaderSource(Shader, 1, &shaderSource, NULL);
    glCompileShader(Shader);

    // check compile status
    glGetShaderiv(Shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << errorLog << std::endl;
    }

    return Shader;
}

unsigned int linkShader(const std::vector<unsigned int>& shaders) {
    int          success;
    char         errorLog[512];
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();

    for (auto& s : shaders) {
        glAttachShader(shaderProgram, s);
    }

    glLinkProgram(shaderProgram);

    // check link status
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, errorLog);
        std::cout << "ERROR::SHADER::LINK_FAILED\n" << errorLog << std::endl;
    }

    return shaderProgram;
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

void renderLoop() {
    // for using shader
    unsigned int shader_program;
    std::vector<unsigned int> shaders;
    shaders.emplace_back(compileShader(vertexShaderSource, GL_VERTEX_SHADER));
    shaders.emplace_back(compileShader(fragmentShaderSource, GL_FRAGMENT_SHADER));

    shader_program = linkShader(shaders);

    // clean shaders
    for (auto& e : shaders) {
        glDeleteShader(e);
    }
    shaders.clear();

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        -0.5f, -0.5f, 0.0f, // left bottom 
         0.5f, -0.5f, 0.0f, // right bottom
         0.5f,  0.5f, 0.0f, // right top   
         -0.5f,  0.5f, 0.0f, // left top   
         // 0.0f,  0.5f, 0.0f,  // top   
    };

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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (void*)0);
    glEnableVertexAttribArray(0);
    // After setting data, unbind objs (use function the same as bind)
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Draw mode
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  // GL_FILL

    // Render loop
    while (!glfwWindowShouldClose(window)) {

        // Process input
        processInput(window);

        // render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // draw triangle
        glUseProgram(shader_program);
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
    glDeleteProgram(shader_program);

    // Clean all assets
    glfwTerminate();
}
