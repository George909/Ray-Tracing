#include "Ray.h"
Ray::Ray() : _r0{ 0, 0, 0, 1 }, _e{ 0, 0, 0, 0 }, _t{ 0 } {}


Ray::Ray(glm::vec4 r0, glm::vec4 e, float t) 
	: _r0{ r0 }, _e{ e }, _t{ t }
{
	_e = glm::normalize(_e);
}

glm::vec4 Ray::r0() const noexcept {
	return _r0;
}

glm::vec4 Ray::e() const noexcept {
	return _e;
}

glm::vec4 Ray::r() const noexcept {
	return _r0 + _t * _e;
}

float Ray::t() const noexcept {
	return _t;
}

void Ray::setR0(glm::vec4 r0) noexcept {
	_r0 = r0;
}

void Ray::setE(glm::vec4 e) noexcept {
	_e = glm::normalize(e);
}


void Ray::setT(float t) noexcept {
	_t = t;
}

Ray::~Ray()
{
}
