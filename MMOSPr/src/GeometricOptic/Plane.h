#pragma once
#include <glm/glm.hpp>
#include "src/GeometricOptic/Ray.h"
class Plane
{
private:
	glm::vec4 _r0;
	glm::vec4 _n;
public:
	Plane(glm::vec4 r0, glm::vec4 n);
	double intersectionPoint(Ray& ray);
	void setR0(glm::vec4 r0);
};

