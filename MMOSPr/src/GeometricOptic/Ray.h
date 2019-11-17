#pragma once
#include <glm/glm.hpp>
class Ray {
protected:
	glm::vec4 _r0;
	float _t;
	glm::vec4 _e;
public:
	Ray();
	Ray(glm::vec4 r0, glm::vec4 e, float t);

	glm::vec4 r0() const noexcept;
	glm::vec4 e() const noexcept;
	glm::vec4 r() const noexcept;
	float t() const noexcept;

	void setR0(glm::vec4 r0) noexcept;
	void setE(glm::vec4 e) noexcept;
	void setT(float t) noexcept;
	~Ray();
};

