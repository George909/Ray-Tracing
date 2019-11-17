#include "Ruler.h"

void Ruler::calculateVertex() {
}

Ruler::Ruler() {

}

Ruler::Ruler(float lenght, float ratio, int countOfDivisions)
	: _lenght{ lenght }, _ratio{ ratio }, _countOfDivisions{ countOfDivisions } 
{
	_vertexData.push_back(-lenght);
	_vertexData.push_back(0);
	_vertexData.push_back(0);

	_vertexData.push_back(lenght);
	_vertexData.push_back(0);
	_vertexData.push_back(0);

	double t = 2 * lenght / countOfDivisions;
	for (double i = -lenght; i <= lenght; i += t) {
		_vertexData.push_back(i);
		_vertexData.push_back(0.02);
		_vertexData.push_back(0);
		
		_vertexData.push_back(i);
		_vertexData.push_back(-0.02);
		_vertexData.push_back(0);

		_vertexData.push_back(i);
		_vertexData.push_back(0);
		_vertexData.push_back(0.02);

		_vertexData.push_back(i);
		_vertexData.push_back(0);
		_vertexData.push_back(-0.02);
	}

	_indexData.push_back(0);
	_indexData.push_back(1);

	for (int i = 2; i < _vertexData.size(); i+=2) {
		_indexData.push_back(i);
		_indexData.push_back(i+1);
	}

}

void Ruler::draw() {
	glBindVertexArray(_vao);
	glDrawElements(GL_LINES, _indexData.size(), GL_UNSIGNED_INT, nullptr);
}

void Ruler::init() {
	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	_vertexBuffer.init(_vertexData.data(), _vertexData.size() * sizeof(float));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
	_indexBuffer.init(_indexData.data(), _indexData.size());

	glBindVertexArray(0);
}

Ruler::~Ruler() {
}
