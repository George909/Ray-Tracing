#pragma once
#include "../OpenGL/IndexBuffer.h"
#include "../OpenGL/VertexBuffer.h"
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
class OpenGLRay {
protected:
	float position[6];
	unsigned int index[2] = { 0, 1 };

	VertexBuffer _vertexBuffer;
	IndexBuffer _indexBuffer;
	unsigned int vao;
public:
	OpenGLRay(glm::vec3 r0, glm::vec3 r);
	OpenGLRay() = default;
	virtual void draw();
	virtual void init();
	~OpenGLRay();
};

