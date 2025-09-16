#include "shader.h"

Shader::Shader(const char* vertexPath, const char* fragmentPath) {
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;

	vShaderFile.exceptions(std::ifstream::badbit | std::ifstream::failbit);
	fShaderFile.exceptions(std::ifstream::badbit | std::ifstream::failbit);

	try {
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();

		vShaderFile.close();
		fShaderFile.close();

		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (std::ifstream::failure e) {
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}

	// Get shader souce content
	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	// Compile shaders
	compileShader_(vShaderCode, GL_VERTEX_SHADER);
	compileShader_(fShaderCode, GL_FRAGMENT_SHADER);
	// Link shaders (Create shader program)
	linkShader_(shaders_);
	// Delete shaders
	deleteShader_(shaders_);
	
}

void Shader::use() {
	glUseProgram(ID);
}

void Shader::setBool(const std::string& name, bool value) const {
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string& name, int value) const {
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) const {
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setVec3(const std::string& name, float a, float b, float c) const {
	glUniform3f(glGetUniformLocation(ID, name.c_str()), a, b, c);
}

void Shader::setVec3(const std::string& name, glm::vec3& v) const {
	this->setVec3(name, v.x, v.y, v.z);
}

void Shader::setMat4(const std::string& name, glm::mat4& trans) const {
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(trans));
}

void Shader::Clean() const {
	glDeleteProgram(this->ID);
}

void Shader::compileShader_(const char* shaderCode, GLenum shaderType) {
	int          success;
	char         infoLog[512];
	unsigned int Shader;

	Shader = glCreateShader(shaderType);
	glShaderSource(Shader, 1, &shaderCode, NULL);
	glCompileShader(Shader);

	// check compile status
	glGetShaderiv(Shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(Shader, 512, NULL, infoLog);

		switch (shaderType) {
		case GL_VERTEX_SHADER:
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
			break;

		case GL_FRAGMENT_SHADER:
			std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
			break;
		}
	}

	shaders_.emplace_back(Shader);
}

void Shader::linkShader_(const std::vector<unsigned int>& shaders) {
	int          success;
	char         infoLog[512];

	ID = glCreateProgram();
	
	for (auto& shader : shaders) {
		glAttachShader(ID, shader);
	}
	glLinkProgram(ID);

	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::LINK_FAILED\n" << infoLog << std::endl;
	}
}

void Shader::deleteShader_(std::vector<unsigned int>& shaders) {
	for (auto& shader : shaders_) {
		glDetachShader(ID, shader);
		glDeleteShader(shader);
	}

	shaders_.clear();
}
