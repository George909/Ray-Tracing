#include "Lense.h"

void Lense::calculateData() {

	_surfaceIndexData_1.clear();
	_surfaceIndexData_2.clear();
	_surfaceVertexData_1.clear();
	_surfaceVertexData_2.clear();

	float PI = 3.14159265358;
	float alphaCount = 72;
	float xCount = 30;
	
	float x1, y1, z1;
	float x2, y2, z2;
	float alphaStep = 2 * PI / alphaCount;
	float xStep1 = (_a_1 - _distance1) / xCount;
	float xStep2 = (_a_2 - _distance2) / xCount;
	float alpha{}, x_1{}, x_2{}, r1{}, r2{};

	for (int i = 0; i <= xCount; ++i) {
		
		x_1 = _a_1 -  i * xStep1;
		x_2 = _a_2 -  i * xStep2;

		for (int j = 0; j <= alphaCount; ++j) {			
			alpha = 2 * PI - j * alphaStep;

			r1 = std::sqrt(1 - (x_1 * x_1 / _a_1 / _a_1)) * _b_1;
			r2 = std::sqrt(1 - (x_2 * x_2 / _a_2 / _a_2)) * _b_2;
			
			x1 = -x_1 + _distance1;
			y1 = r1 * std::cosf(alpha);
			z1 = r1 * std::sinf(alpha);
			_surfaceVertexData_1.push_back(x1);
			_surfaceVertexData_1.push_back(y1);
			_surfaceVertexData_1.push_back(z1);

			x2 = x_2 - _distance2;
			y2 = r2 * std::cosf(alpha);
			z2 = r2 * std::sinf(alpha);
			_surfaceVertexData_2.push_back(x2);
			_surfaceVertexData_2.push_back(y2);
			_surfaceVertexData_2.push_back(z2);
		}
	}

	int k1{}, k2{};
	for (int i = 0; i < xCount; ++i) {
		k1 = i * (alphaCount + 1);
		k2 = k1 + alphaCount + 1;

		for (int j = 0; j < alphaCount; j++, k1++, k2++) {

			if (i != 0) {
				_surfaceIndexData_1.push_back(k1);
				_surfaceIndexData_1.push_back(k2);
				_surfaceIndexData_1.push_back(k1 + 1);

				_surfaceIndexData_2.push_back(k1);
				_surfaceIndexData_2.push_back(k2);
				_surfaceIndexData_2.push_back(k1 + 1);
			}

			if (i != (xCount - 1)) {
				_surfaceIndexData_1.push_back(k1 + 1);
				_surfaceIndexData_1.push_back(k2);
				_surfaceIndexData_1.push_back(k2 + 1);

				_surfaceIndexData_2.push_back(k1 + 1);
				_surfaceIndexData_2.push_back(k2);
				_surfaceIndexData_2.push_back(k2 + 1);
			}
		}
	}
}

void Lense::refraction(RayArray& rayArray, RayArray& inRays,	RayArray& outRays) {
	
	for (unsigned int i = 0; i < rayArray.size(); i++) {
		if (rayArray(i).r().y * rayArray(i).r().y + rayArray(i).r().z * rayArray(i).r().z > _height* _height) {
			rayArray(i).setT(0);
			continue;
		}
		auto t = intersectionPoint_1(rayArray(i));
		if (!t.has_value()) continue;

		float temp = glm::dot(rayArray(i).e(), currentNormal);
		double denominator = temp * temp * _n1 * _n1;
		double numerator = _n2 * _n2 - _n1 * _n1;
		float bigSqrt = std::sqrt(numerator / denominator + 1);
		glm::vec4 e = (_n1 * rayArray(i).e() - temp * currentNormal * _n1 * (1 - bigSqrt)) / _n2;

		inRays.push_back(Ray(rayArray(i).r(), e, t.value()));
	}

	for (int i = 0; i < inRays.size(); i++) {
		auto t = intersectionPoint_2(inRays(i));
		if (!t.has_value()) continue;

		float temp = glm::dot(inRays(i).e(), currentNormal);
		double denominator = temp * temp * _n2 * _n2;
		double numerator = _n1 * _n1 - _n2 * _n2;
		double underSqrt = numerator / denominator + 1;
		if (underSqrt < 0) continue;
		float bigSqrt = std::sqrt(underSqrt);
		glm::vec4 e = (_n2 * inRays(i).e() - temp * currentNormal * _n2 * (1 - bigSqrt)) / _n1;

		outRays.push_back(Ray(inRays(i).r(), e, t.value()));
	}
}

std::optional<Ray> Lense::reflection(Ray& ray) {
	//auto t = intersectionPoint(ray);
	//if (!t.has_value())
	//	return std::nullopt;

	//glm::vec4 e = ray.e() - 2 * glm::dot(ray.e(), currentNormal) * currentNormal;
	//return Ray(ray.r(), e, t.value());
	return {};
}


glm::vec4 Lense::n1(Ray& ray) {
	glm::vec4 normal = { 2 * ray.r().x / _a_1 / _a_1,
											 2 * ray.r().y / _b_1 / _b_1,
											 2 * ray.r().z / _c_1 / _c_1,
																							 0 };
	return glm::normalize(normal);
}

glm::vec4 Lense::n2(Ray& ray) {
	glm::vec4 normal = { 2 * ray.r().x / _a_2 / _a_2,
											 2 * ray.r().y / _b_2 / _b_2,
											 2 * ray.r().z / _c_2 / _c_2,
																							 0 };
	return glm::normalize(normal);
}

std::optional<double> Lense::intersectionPoint_1(Ray& ray) {
	glm::mat3x3 matrix(_b_1 * _c_1, 0., 0.,
		0., _a_1 * _c_1, 0.,
		0., 0., _b_1 * _a_1);

	glm::vec3 e(ray.e()), r0(ray.r0()), ellipsoid_r0(_r0_1);

	double denominator = glm::dot(matrix * e, matrix * e);
	double numerator = glm::dot(matrix * e, matrix * (r0 - ellipsoid_r0));

	double big = numerator * numerator - denominator *
		(glm::dot(matrix * (r0 - ellipsoid_r0), matrix * (r0 - ellipsoid_r0)) - glm::determinant(matrix));

	if (big < 0. || big == 0.f)
		return std::nullopt;

	double bigSqrt = std::sqrt(big);
	if (comparingDoubleWithZero(bigSqrt)) {
		if (-numerator / denominator < 0.) {
			return std::nullopt;
		}
		else {
			ray.setT(-numerator / denominator);
			currentNormal = n1(ray);
			return -numerator / denominator;
		}
	}

	double t1{}, t2{};
	t1 = (-numerator - bigSqrt) / denominator;
	t2 = (-numerator + bigSqrt) / denominator;

	if ((t1 < 0 && t2 < 0))
		return std::nullopt;

	if ((t1 > 0 && t2 < 0) || (t1 < 0 && t2 > 0)) {
		float t = std::max(t1, t2);
		ray.setT(t);
		currentNormal = n1(ray);
		return t;
	}
	float t = std::min(t1, t2);
	ray.setT(t);
	currentNormal = n1(ray);
	return t;
}

std::optional<double> Lense::intersectionPoint_2(Ray& ray) {
	glm::mat3x3 matrix(_b_2 * _c_2, 0., 0.,
											0., _a_2 * _c_2, 0.,
											0., 0., _b_2 * _a_2);

	glm::vec3 e(ray.e()), r0(ray.r0()), ellipsoid_r0(_r0_2);

	double denominator = glm::dot(matrix * e, matrix * e);
	double numerator = glm::dot(matrix * e, matrix * (r0 - ellipsoid_r0));

	double big = numerator * numerator - denominator *
		(glm::dot(matrix * (r0 - ellipsoid_r0), matrix * (r0 - ellipsoid_r0)) - glm::determinant(matrix));

	if (big < 0. || big == 0.f)
		return std::nullopt;

	double bigSqrt = std::sqrt(big);
	if (comparingDoubleWithZero(bigSqrt)) {
		if (-numerator / denominator < 0.) {
			return std::nullopt;
		}
		else {
			ray.setT(-numerator / denominator);
			currentNormal = -n2(ray);
			return -numerator / denominator;
		}
	}

	double t1{}, t2{};
	t1 = (-numerator - bigSqrt) / denominator;
	t2 = (-numerator + bigSqrt) / denominator;

	if ((t1 < 0 && t2 < 0))
		return std::nullopt;

	if ((t1 > 0 && t2 < 0) || (t1 < 0 && t2 > 0)) {
		float t = std::max(t1, t2);
		ray.setT(t);
		currentNormal = -n2(ray);
		return t;
	}

	float t = std::min(t1, t2);
	ray.setT(t);
	currentNormal = -n2(ray);
	return t;
}

bool Lense::comparingDoubleWithZero(double& value) const noexcept {
	return value <= 1e-8 && value >= -1e-8;
}

void Lense::initLense() {
	float a1, a2, b1, b2;
	a1 = _a_1 * _a_1;
	a2 = _a_2 * _a_2;
	b1 = _b_1 * _b_1;
	b2 = _b_2 * _b_2;
	double c = _a_1 + _a_2 - _distance;

	double bigSqrt = std::sqrt(a1 * a2 * (a2 * (b1 * b1 - b1 * b2) + b2 * (a1 * (-b1 + b2) + b1 * c * c)));
	_distance2 = (a2 * b1 * c - bigSqrt) / (a2 * b1 - a1 * b2);
	_distance1 = c - _distance2;

	_height = std::sqrt(1 - (_distance2 * _distance2 / _a_2 / _a_2)) * _b_2;

	//double bigSqrt = std::sqrt(a1 * a2 * (a2 * b1 * b1 + 2 * _a_2 * b1 * b2 * (_a_1 - _distance) +
	//	b2 * (a1 * b2 - 2 * _a_1 * b1 * _distance + b1 * _distance * _distance)));
	//_distance2 = (_a_1 * a2 * b2 + _a_2 * a2 * b1 - a2 * b1 * _distance - bigSqrt) / (a2 * b1 - a1 * b2);

	_r0_1 = { _distance1, 0.f, 0.f, 1.f };
	_r0_2 = { -_distance2, 0.f, 0.f, 1.f };
}

void Lense::setA1(float a_1) {
	_a_1 = a_1;
	initLense();
}

void Lense::setA2(float a_2) {
	_a_2 = a_2;
	initLense();
}

void Lense::setB1(float b_1) {
	_b_1 = b_1;
	_c_1 = b_1;
	initLense();
}

void Lense::setB2(float b_2) {
	_b_2 = b_2;
	_c_2 = b_2;
	initLense();
}

void Lense::setDistance(float distance) {
	_distance = distance;
	initLense();
}

float Lense::getDistance1()
{
	return _distance1;
}

float Lense::getDistance2()
{
	return _distance2;
}

float Lense::getDistance()
{
	return _distance;
}

Lense::Lense(float r_1, float r_2, float distance, float n1, float n2)
	: _r_1{ r_1 }, _r_2{r_2}, _distance{ distance }, _n1{ n1 }, _n2{ n2 }
{
	float r1 = std::abs(_r_1);
	float r2 = std::abs(_r_2);
	_distance2 = (_distance * (_distance - 2 * r1)) / (2 * (_distance - r1 - r2));
	_distance1 = (_distance * (_distance - 2 * r2)) / (2 * (_distance - r1 - r2));

	_r0_1 = _r_1 > 0 
		? glm::vec4(_r_1 - _distance1, 0.f, 0.f, 1.f) 
		: glm::vec4(-_r_1 - _distance1, 0.f, 0.f, 1.f);
	_r0_2 = _r_2 > 0 
		? glm::vec4(-_r_2 + _distance2, 0.f, 0.f, 1.f) 
		: glm::vec4(_r_2 + _distance2, 0.f, 0.f, 1.f);

	calculateData();
}

Lense::Lense(float a_1, float b_1, float c_1, float a_2, float b_2, float c_2, float distance, float n1, float n2)
	: _a_1{ a_1 }, _b_1{ b_1 }, _c_1{ c_1 }, _a_2{ a_2 }, _b_2{ b_2 }, _c_2{ c_2 }, _distance{ distance }, _n1{ n1 }, _n2{ n2 }
{
	initLense();
}

void Lense::draw() {
	glBindVertexArray(vao1);
	glDrawElements(GL_LINES, _surfaceIndexData_1.size(), GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(vao2);
	glDrawElements(GL_LINES, _surfaceIndexData_2.size(), GL_UNSIGNED_INT, nullptr);
}

void Lense::init() {
	calculateData();
	glGenVertexArrays(1, &vao1);
	glBindVertexArray(vao1);

	_vertexBuffer1.init(_surfaceVertexData_1.data(), _surfaceIndexData_1.size() * sizeof(float));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
	_indexBuffer1.init(_surfaceIndexData_1.data(), _surfaceIndexData_1.size());

	glBindVertexArray(0);

	glGenVertexArrays(1, &vao2);
	glBindVertexArray(vao2);

	_vertexBuffer2.init(_surfaceVertexData_2.data(), _surfaceIndexData_2.size() * sizeof(float));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
	_indexBuffer2.init(_surfaceIndexData_2.data(), _surfaceIndexData_2.size());

	glBindVertexArray(0);

}
