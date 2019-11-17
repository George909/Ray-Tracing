#include "Plane.h"

Plane::Plane(glm::vec4 r0, glm::vec4 n)
	: _r0{ r0 }, _n{ n }
{
}

double Plane::intersectionPoint(Ray& ray) {
	
	return glm::dot(_n, _r0 - ray.r0()) / glm::dot(_n, ray.e());
}

void Plane::setR0(glm::vec4 r0)
{
	_r0 = r0;
}
