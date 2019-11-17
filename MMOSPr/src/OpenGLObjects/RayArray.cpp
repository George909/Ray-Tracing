#include "RayArray.h"

RayArray::RayArray(glm::vec4 position, glm::vec4 e, unsigned int count, double radius)
	: _position{ position }, _e{ e }
{
	_rays.resize(count);
	float M_PI = 3.14f;
	std::uniform_real_distribution<float> distAlpha(-M_PI, M_PI);
	std::uniform_real_distribution<float> distRadius(0, radius);
	std::default_random_engine generator;
	float y{}, z{};

	for (int i = 0; i < count; i++) {
		y = distRadius(generator) * std::cos(distAlpha(generator));
		z = distRadius(generator) * std::sin(distAlpha(generator));

		_rays[i] = Ray({ _position.x, _position.y + y, _position.z + z, 1.f }, e, 0.2f);
	}
}

RayArray::RayArray()
	: _position{ 0 }, _e{ 1,0,0,0 }
{

}

Ray& RayArray::operator()(unsigned int i)
{
	return _rays[i];
}

unsigned int RayArray::size() const noexcept
{
	return _rays.size();
}

void RayArray::init() {
	_vertexData.clear();
	_indexData.clear();

	for (int i = 0; i < _rays.size(); i++) {
		_vertexData.push_back(_rays[i].r0().x);
		_vertexData.push_back(_rays[i].r0().y);
		_vertexData.push_back(_rays[i].r0().z);

		_vertexData.push_back(_rays[i].r().x);
		_vertexData.push_back(_rays[i].r().y);
		_vertexData.push_back(_rays[i].r().z);
	}

	for (int i = 0; i < _rays.size() * 2; i += 2) {
		_indexData.push_back(i);
		_indexData.push_back(i + 1);
	}

	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	_vertexBuffer.init(_vertexData.data(), _vertexData.size() * sizeof(float));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
	_indexBuffer.init(_indexData.data(), _indexData.size());

	glBindVertexArray(0);
}

void RayArray::draw() {
	glBindVertexArray(_vao);
	glDrawElements(GL_LINES, _indexData.size(), GL_UNSIGNED_INT, nullptr);
}

void RayArray::push_back(Ray ray) {
	_rays.push_back(ray);
}

void RayArray::clear() {
	_rays.clear();
}
