#pragma once

#include <gl/glew.h>

class VertexBuffer {
protected:
	unsigned int rendererID;
public:
	VertexBuffer(const float* data, unsigned int size);
	VertexBuffer() = default;
	void init(const float* data, unsigned int size);
	void bind() const;
	void unBind() const;

	~VertexBuffer();
};

