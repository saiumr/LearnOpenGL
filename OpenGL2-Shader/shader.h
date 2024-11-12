/*
 * 从文件读取顶点数据的Shader类
 */

#pragma once
#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

class Shader {
public:
	// program id
	unsigned int ID;

	Shader(const char* vertexPath, const char* fragmentPath);

	void use();
	// uniform tool function
	void setBool(const std::string& name, bool value)   const;
	void setInt(const std::string& name, int value)     const;
	void setFloat(const std::string& name, float value) const;

private:
	void compileShader_(const char* shaderCode, GLenum shaderType);
	void linkShader_(const std::vector<unsigned int>& shaders);
	void deleteShader_(std::vector<unsigned int>& shaders);
	std::vector<unsigned int> shaders_;
};
