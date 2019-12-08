#pragma once
#include <gl/glew.h>
#include "../OpenGL/IndexBuffer.h"
#include "../OpenGL/VertexBuffer.h"

#include "../GeometricOptic/ray.h"
#include <glm/glm.hpp>
#include <vector>
#include <random>
#include <fstream>


class RayArray
{
	std::vector<Ray> _rays;
	glm::vec4 _position;
	glm::vec4 _e;

	float _r;
	unsigned int _c;

	std::vector<float> _vertexData;
	std::vector<unsigned int> _indexData;
	unsigned int _vao;
	VertexBuffer _vertexBuffer;
	IndexBuffer _indexBuffer;

	void initRays();

public:
	RayArray(glm::vec4 position, glm::vec4 e,unsigned int count, float radius);
	RayArray();
	Ray& operator()(unsigned int i);
	unsigned int size() const noexcept;
	void setR(double r);
	void setCount(double count);
	void push_back(Ray ray);
	void clear();

	void scatterPlot(const std::string filename, double distance);

	void init();
	void draw();
};

