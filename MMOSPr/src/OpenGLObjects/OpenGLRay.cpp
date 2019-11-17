#include "OpenGLRay.h"

OpenGLRay::OpenGLRay(glm::vec3 r0, glm::vec3 r){
	position[0] = r0.x;
	position[1] = r0.y;
	position[2] = r0.z;
	position[3] = r.x;
	position[4] = r.y;
	position[5] = r.z;
}

void OpenGLRay::draw() {
	glBindVertexArray(vao);
	glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, nullptr);
}

void OpenGLRay::init() {
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	_vertexBuffer.init(position, 6 * sizeof(float));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	_indexBuffer.init(index, 2);

	glBindVertexArray(0);
}

OpenGLRay::~OpenGLRay() {
}
