#pragma once
#include <gl/glew.h>

class IndexBuffer {
protected:
	unsigned int rendererID;
	unsigned int count;
public:
	IndexBuffer(const unsigned int* data, unsigned int count);
	IndexBuffer() = default;
	
	void bind() const;
	void init(const unsigned int* data, unsigned int count);
	void unBind() const;

	~IndexBuffer();
};

