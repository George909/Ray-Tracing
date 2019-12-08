#pragma once
#include <gl/glew.h>
#include <vector>
#include "../OpenGL/IndexBuffer.h"
#include "../OpenGL/VertexBuffer.h"

class Ruler {
private:
	float _lenght;
	float _ratio;
	int _countOfDivisions;
	void calculateVertex();

	std::vector<float> _vertexData;
	std::vector<unsigned int> _indexData;
	IndexBuffer _indexBuffer;
	VertexBuffer _vertexBuffer;
	unsigned int _vao;
public:
	Ruler();
	Ruler(float lenght, float ratio, int countOfDivisions);

	virtual void draw() ;
	virtual void init() ;

	~Ruler();
};

