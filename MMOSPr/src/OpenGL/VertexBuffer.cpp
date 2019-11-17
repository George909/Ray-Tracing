#include "VertexBuffer.h"

VertexBuffer::VertexBuffer(const float* data, unsigned int size) {
	init(data, size);
}

void VertexBuffer::init(const float* data, unsigned int size) {
	glGenBuffers(1, &rendererID);
	glBindBuffer(GL_ARRAY_BUFFER, rendererID);
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

void VertexBuffer::bind() const {
	glBindBuffer(GL_ARRAY_BUFFER, rendererID);
}

void VertexBuffer::unBind() const {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

VertexBuffer::~VertexBuffer() {
	glDeleteBuffers(1, &rendererID);
}
