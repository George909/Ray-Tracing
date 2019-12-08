#include "Lense.h"
#include <iostream>

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
											 2 * ray.r().z / _b_1 / _b_1,
																							 0 };
	return glm::normalize(normal);
}

glm::vec4 Lense::n2(Ray& ray) {
	glm::vec4 normal = { 2 * ray.r().x / _a_2 / _a_2,
											 2 * ray.r().y / _b_2 / _b_2,
											 2 * ray.r().z / _b_2 / _b_2,
																							 0 };
	return glm::normalize(normal);
}

std::optional<double> Lense::intersectionPoint_1(Ray& ray) {
	glm::mat3x3 matrix(_b_1 * _b_1, 0., 0.,
		0., _a_1 * _b_1, 0.,
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
	glm::mat3x3 matrix(_b_2 * _b_2, 0., 0.,
											0., _a_2 * _b_2, 0.,
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

	if ((a1 == a2 && b1 == b2) || (a2 * b1 - a1 * b2) == 0 ) {
		_distance1 = _distance2 = _distance2 / 2.;
	}
	else {
		double bigSqrt = std::sqrt(a1 * a2 * (a2 * (b1 * b1 - b1 * b2) + b2 * (a1 * (-b1 + b2) + b1 * c * c)));
		_distance2 = (a2 * b1 * c - bigSqrt) / (a2 * b1 - a1 * b2);
		_distance1 = c - _distance2;
	}

	_height = std::sqrt(1 - (_distance2 * _distance2 / _a_2 / _a_2)) * _b_2;

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
	_b_1 = b_1;
	initLense();
}

void Lense::setB2(float b_2) {
	_b_2 = b_2;
	_b_2 = b_2;
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

float Lense::getA1()
{
	return _a_1;
}

float Lense::getA2()
{
	return _a_2;
}

float Lense::getB1()
{
	return _b_1;
}

float Lense::getB2()
{
	return _b_2;
}

void Lense::calculateLense(float focus, 
													 float lb_a1, float rb_a1, 
													 float lb_b1, float rb_b1, 
													 float lb_a2, float rb_a2,
													 float lb_b2, float rb_b2, 
													 float lb_distance, float rb_distance) {
	struct LenseParametrs {
		LenseParametrs()
			: _a1{}, _a2{}, _b1{}, _b2{}, _d{}, _mse{} {};
		LenseParametrs(float a1, float a2, float b1, float b2, float d, float mse)
			: _a1{ a1 }, _a2{ a2 }, _b1{ b1 }, _b2{ b2 }, _d{ d }, _mse{ mse } {};
		float _a1;
		float _b1;
		float _a2;
		float _b2;
		float _d;
		float _mse;
	};

	float step1 = 6;
	float step2 = 6;
	  
	LenseParametrs start(lb_a1, lb_a2, lb_b1, lb_b2, lb_distance, std::numeric_limits<float>().max());
	LenseParametrs end(rb_a1, rb_a2, rb_b1, rb_b2, rb_distance, std::numeric_limits<float>().max());
	LenseParametrs temp;

	std::vector<LenseParametrs> params;
	float stepD = (end._d - start._d) / step1;
	float stepA1 = (end._a1 - start._a1) / step2;
	float stepA2 = (end._a2 - start._a2) / step2;
	float stepB1 = (end._b1 - start._b1) / step2;
	float stepB2 = (end._b2 - start._b2) / step2;
	unsigned int i = 0;
	
	for (temp._d = start._d; temp._d < end._d; temp._d += stepD) {
		for (temp._a1 = start._a1; temp._a1 < end._a1; temp._a1 += stepA1) {
			for (temp._a2 = start._a2; temp._a2 < end._a2; temp._a2 += stepA2) {
				for (temp._b1 = start._b1; temp._b1 < end._b1; temp._b1 += stepB1) {
					for (temp._b2 = start._b2; temp._b2 < end._b2; temp._b2 += stepB2) {
						
						_a_1 = temp._a1;
						_a_2 = temp._a2;
						_b_1 = temp._b1;
						_b_2 = temp._b2;
						_distance = temp._d;
						initLense();
						calculateFocus();
						i++;
						std::cout << i << std::endl;
						if (_f < focus + 1e-2 && _f > focus - 1e-2) {
							std::cout << "pedofil" << std::endl;
							temp._mse = _mse;
							params.push_back(temp);
						}
					}
				}
			}
		}
	}

	double minMSE = std::numeric_limits<float>().max();
	int minMSEIndex = 0;

	for (int i = 0; i < params.size(); i++) {
		if (params[i]._mse < minMSE) {
			minMSE = params[i]._mse;
			minMSEIndex = i;
		}
	}

	_a_1 = params[minMSEIndex]._a1;
	_a_2 = params[minMSEIndex]._a2;
	_b_1 = params[minMSEIndex]._b1;
	_b_2 = params[minMSEIndex]._b2;
	_distance = params[minMSEIndex]._d;
	initLense();

	/*float start[] = { lb_distance, lb_b1, lb_b2, lb_a1, lb_a2};
	float end[] = { rb_distance, rb_b1, rb_b2, rb_a1, rb_a2 };
	float *p[] = { &_distance, &_b_1, &_b_2, &_a_1, &_a_2 };
	float startF{}, midF{}, endF{}, tempF{std::numeric_limits<float>().max()};*/

	//for (int i = 0; i < 5; i++) {
	//	*p[i] = (end[i] + start[i]) / 2.;
	//}
	
	//for (int i = 0; i < 3; i++) {
	//	std::cout << " ";
	//	while (true)
	//	{
	//		*p[i] = start[i];
	//		initLense();
	//		calculateFocus();
	//		startF = _f;

	//		*p[i] = end[i];
	//		initLense();
	//		calculateFocus();
	//		endF = _f;

	//		*p[i] = (end[i] + start[i]) / 2.;
	//		initLense();
	//		calculateFocus();

	//		if (focus < startF) {
	//			*p[i] = start[i];
	//			break;
	//		}
	//		if (focus > endF) {
	//			*p[i] = end[i];
	//			break;
	//		}


	//		std::cout << _f << "Tbl nu6p" << i << std::endl;
	//		if (focus > midF + 1e-2) {
	//			start[i] = *p[i];
	//			continue;
	//		}
	//		if (focus < midF - 1e-2) {
	//			end[i] = *p[i];
	//			continue;
	//		}
	//		break;
	//	}
	//}
	//for (int i = 3; i < 5; i++) {
	//	while (true)
	//	{
	//		*p[i] = start[i];
	//		initLense();
	//		calculateFocus();
	//		startF = _f;

	//		*p[i] = end[i];
	//		initLense();
	//		calculateFocus();
	//		endF = _f;

	//		*p[i] = (end[i] + start[i]) / 2.;
	//		initLense();
	//		calculateFocus();
	//		midF = _f;

	//		if (focus < startF) {
	//			*p[i] = start[i];
	//			break;
	//		}
	//		if (focus > endF) {
	//			*p[i] = end[i];
	//			break;
	//		}

	//		std::cout << _f << "ты пидр" << std::endl;
	//		if (focus > midF + 1e-2) {
	//			end[i] = *p[i];
	//			continue;
	//		}
	//		if (focus < midF - 1e-2) {
	//			start[i] = *p[i];
	//			continue;
	//		}
	//		break;
	//	}
	//}

}

float Lense::calculateFocus() {
	Plane plane({ _distance1, 0, 0, 1 }, { 1, 0, 0,0 });

	RayArray	rays = RayArray({ -2.f, 0.f, 0.f, 1 },
														{ 1.f, 0.f, 0.f, 0.f },
														50, 0.1f);

	RayArray inRays, outRays;
	refraction(rays, inRays, outRays);
	for (int i = 0; i < outRays.size(); i++) {
		outRays(i).setT(10);
	}

	double currentMSE{};
	double lastMSE = std::numeric_limits<double>::max();
	double step = _distance / 10;
	double currentStep = _distance2;
	double t{};
	double li{};
	double minMSE = std::numeric_limits<double>::max();

	for (;currentStep < 10; currentStep += step) {
		plane.setR0({ currentStep, 0, 0, 1 });
		for (int i = 0; i < outRays.size(); i++) {
			t = plane.intersectionPoint(outRays(i));
			outRays(i).setT(t);
		}

		for (int i = 0; i < outRays.size(); i++) {
			currentMSE += std::sqrt((outRays(i).r().y * outRays(i).r().y + outRays(i).r().z * outRays(i).r().z) / 
				outRays.size());
		}
		if (lastMSE <= currentMSE) {
			plane.setR0({ currentStep - step, 0, 0, 1 });
			for (int i = 0; i < outRays.size(); i++) {
				t = plane.intersectionPoint(outRays(i));
				outRays(i).setT(t);
			}
			_f = currentStep - step;
			return currentStep - step;
		}
		_mse = lastMSE = currentMSE;
		currentMSE = 0;
	}
}

Lense::Lense(float a_1, float b_1, float c_1, float a_2, float b_2, float c_2, float distance, float n1, float n2)
	: _a_1{ a_1 }, _b_1{ b_1 }, _a_2{ a_2 }, _b_2{ b_2 }, _distance{ distance }, _n1{ n1 }, _n2{ n2 }
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
