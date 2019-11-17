#pragma once
#include <gl/glew.h>
#include <string>
#include <iostream>
#include <unordered_map>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader {
private:
	std::string _filePath;
	unsigned int _shaderID;
	std::unordered_map<std::string, unsigned int> _uniformLcationCache;
	unsigned int getUniformLocation(const std::string& name);
	int compileShader(unsigned int type, const std::string& shader);
	void createShader(const std::string& vertexShader, const std::string& fragmentShader);
public:
	Shader();
	Shader(const std::string& filepath);

	void bind() const;
	void unBind() const;

	//Set unfiorms
	void setUniform4f(const std::string& name, float v1, float v2, float v3, float v4);
	void setUniformMatrix4fv(const std::string& name, const glm::mat4& mat4);

	~Shader();
};

