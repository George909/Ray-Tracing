#include "Shader.h"

int Shader::compileShader(unsigned int type, const std::string& shader) {
	unsigned int id = glCreateShader(type);
	const char* src = shader.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		int lenght;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &lenght);
		char* log = static_cast<char*>(malloc(lenght * sizeof(char)));
		glGetShaderInfoLog(id, lenght, &lenght, log);
		std::cout << "Failed to compile "
			<< (type == GL_VERTEX_SHADER ? "vertex" : "fragment")
			<< "shader!" << std::endl
			<< log << std::endl;
		free(log);
		glDeleteShader(id);
		return 0;
	}

	return id;
}

void Shader::createShader(const std::string& vertexShader, const std::string& fragmentShader) {
	_shaderID = glCreateProgram();
	unsigned int vShader = compileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fShader = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(_shaderID, vShader);
	glAttachShader(_shaderID, fShader);
	glLinkProgram(_shaderID);
	glValidateProgram(_shaderID);

	glDeleteShader(vShader);
	glDeleteShader(fShader);
}

Shader::Shader()
{
	std::string vertexShader =
		"#version 330 core\n"
		"layout(location = 0) in vec4 position;\n"
		"uniform mat4 model;\n"
		"uniform mat4 view;\n"
		"uniform mat4 projection;\n"
		"void main()\n"
		"{\n"
		"gl_Position = projection * view * model * position;\n"
		"}\n";

	std::string fragmentShader =
		"#version 330 core\n"
		"layout(location = 0) out vec4 color;\n"
		"uniform vec4 u_color;\n"
		"void main()\n"
		"{\n"
		"color = u_color;\n"
		"}\n";

	createShader(vertexShader, fragmentShader);
}

Shader::Shader(const std::string& filepath) {
}

void Shader::bind() const {
	glUseProgram(_shaderID);
}

void Shader::unBind() const {
	glUseProgram(0);
}

unsigned int Shader::getUniformLocation(const std::string& name) {
	if (_uniformLcationCache.find(name) != _uniformLcationCache.end())
		return _uniformLcationCache[name];

	unsigned int location = glGetUniformLocation(_shaderID, name.c_str());
	if (location == -1) {
		std::cout << "Warning: Uniform " << name << std::endl;
		return -1;
	}
	_uniformLcationCache[name] = location;
	return location;
}

void Shader::setUniform4f(const std::string& name, float v1, float v2, float v3, float v4) {
	glUniform4f(getUniformLocation(name), v1, v2, v3, v4);
}

void Shader::setUniformMatrix4fv(const std::string& name, const glm::mat4& mat4) {
	glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(mat4));
}

Shader::~Shader() {
}
