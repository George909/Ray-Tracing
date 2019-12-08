#pragma once

#include "../OpenGL/IndexBuffer.h"
#include "../OpenGL/VertexBuffer.h"

#include "../GeometricOptic/Ray.h"
#include "../GeometricOptic/Plane.h"
#include "RayArray.h"

#include <gl/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <optional>
#include <cmath>
#include <algorithm>


class Lense {

private:
	std::vector<float> _surfaceVertexData_1;
	std::vector<unsigned int> _surfaceIndexData_1;
	std::vector<float> _surfaceVertexData_2;
	std::vector<unsigned int> _surfaceIndexData_2;
	
	unsigned int vao1;
	unsigned int vao2;
	VertexBuffer _vertexBuffer1;
	IndexBuffer _indexBuffer1;
	VertexBuffer _vertexBuffer2;
	IndexBuffer _indexBuffer2;

	float _a_1;
	float _b_1;
	float _a_2;
	float _b_2;

	float _distance;
	float _distance1;
	float _distance2;
	float _height;

	glm::vec4 _r0_1;
	glm::vec4 _r0_2;
	float _n1;
	float _n2;
	float _f;
	float _mse;

	void calculateData();
	glm::vec4 n1(Ray& ray);
	glm::vec4 n2(Ray& ray);
	std::optional<double> intersectionPoint_1(Ray& ray);
	std::optional<double> intersectionPoint_2(Ray& ray);
	bool comparingDoubleWithZero(double& value) const noexcept;

	void initLense();

	glm::vec4 currentNormal;

public:

	Lense(float a_1, float b_1, float c_1, float a_2, float b_2, float c_2, float distance, float n1, float n2);

	void refraction(RayArray& rayArray, RayArray& inRays, RayArray& outRays);
	std::optional<Ray> reflection(Ray& ray);
	
	void setA1(float a_1);
	void setA2(float a_2);
	void setB1(float b_1);
	void setB2(float b_2);
	void setDistance(float distance);
	float getDistance1();
	float getDistance2();
	float getDistance();
	float getA1();
	float getA2();
	float getB1();
	float getB2();

	void calculateLense(float focus, 
											float lb_a1, float rb_a1, 
											float lb_b1, float rb_b1, 
											float lb_a2, float rb_a2, 
											float lb_b2, float rb_b2, 
											float lb_distance, float rb_distance);
	float calculateFocus();

	// Унаследовано через Objec
	virtual void draw();
	virtual void init();
};