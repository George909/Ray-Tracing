#pragma once
#include <gl/glew.h>
#include "../OpenGL/IndexBuffer.h"
#include "../OpenGL/VertexBuffer.h"

#include "../GeometricOptic/ray.h"
#include <glm/glm.hpp>
#include <vector>
#include <random>

class RayArray
{
	std::vector<Ray> _rays;
	glm::vec4 _position;
	glm::vec4 _e;

	std::vector<float> _vertexData;
	std::vector<unsigned int> _indexData;
	unsigned int _vao;
	VertexBuffer _vertexBuffer;
	IndexBuffer _indexBuffer;

public:
	RayArray(glm::vec4 position, glm::vec4 e,unsigned int count, double radius);
	RayArray();
	Ray& operator()(unsigned int i);
	unsigned int size() const noexcept;

	void init();
	void draw();

	void push_back(Ray ray);
	void clear();
};

