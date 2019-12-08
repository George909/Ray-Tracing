#pragma once
#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <string>
#include <optional>
#include <random>

#include "OpenGL/IndexBuffer.h"
#include "OpenGL/VertexBuffer.h"
#include "OpenGL/Shader.h"

#include "OpenGLObjects/Lense.h"
#include "OpenGLObjects/RayArray.h"
#include "OpenGLObjects/Ruler.h"
#include "GeometricOptic/Plane.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_glfw.h"
#include "ImGui/imgui_impl_opengl3.h"

double M_PI = 3.141592653589793238;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void do_movement();
bool interactive = false;
const unsigned int WIDTH = 1600, HEIGHT = 900;

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float yaw = -90.0f;	
float pitch = 0.0f;
float lastX = WIDTH / 2.0;
float lastY = HEIGHT / 2.0;
bool keys[1024];

// Deltatime
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f;  	// Time of last frame

float calculateFocus(RayArray& rays, Lense& lense);
void calculateLens(float focus);
int main() {
	GLFWwindow* window;
	if (!glfwInit()) return -1;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	window = glfwCreateWindow(WIDTH, HEIGHT, "Ray Tracing", nullptr, nullptr);
	if (!window) {
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	if (glewInit() != GLEW_OK) return -1;
	glfwSetKeyCallback(window, key_callback);
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSwapInterval(1);
	glViewport(0, 0, WIDTH, HEIGHT);
	glEnable(GL_DEPTH_TEST);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 130");
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	RayArray rays;
	rays = RayArray({ -2.f, 0.f, 0.f, 1 }, 
									{ 1.f, 0.f, 0.f, 0.f }, 
															 100, 0.3f);

	Lense lense(0.4f, 1.5, 1.5, 0.5f, 1.5, 1.5, 0.25f, 1.f, 1.5f);
	Ruler ruler(5, 0.1, 20);

	RayArray inRays, outRays;
	lense.refraction(rays, inRays, outRays);
	for (int i = 0; i < outRays.size(); i++) {
		outRays(i).setT(10);
	}

	lense.init();
	ruler.init();
	rays.init();
	inRays.init();
	outRays.init();

	Shader shader;

	shader.unBind();
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	
	float a1{ 0.4f }, a2{ 0.5f }, b1{ 1.5f }, b2{ 1.5f }, d{ 0.25f }, focus{}, radius{0.3}, count{100};
	char* filename;
	while (!glfwWindowShouldClose(window)) {

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		// Lense and Rays
		{
			static float f = 0.0f;
			static int counter = 0;

			ImGui::Begin("Lense and Rays");                          
  
			ImGui::Checkbox("Interactive", &interactive);
		
			ImGui::InputFloat("a1", &a1);
			ImGui::InputFloat("b1", &b1);
			ImGui::InputFloat("a2", &a2);
			ImGui::InputFloat("b2", &b2);
			ImGui::InputFloat("Distance", &d);
			ImGui::InputFloat("Count", &count);
			ImGui::InputFloat("Radius", &radius);

			if (ImGui::Button("Refresh")) {
				lense.setA1(a1);
				lense.setA2(a2);
				lense.setB1(b1);
				lense.setB2(b2);
				lense.setDistance(d);
				rays.setCount(count);
				rays.setR(radius);
				inRays.clear();
				outRays.clear();
				lense.refraction(rays, inRays, outRays);
				for (int i = 0; i < outRays.size(); i++) {
					outRays(i).setT(10);
				}
				rays.init();
				inRays.init();
				outRays.init();
				lense.init();
			}

			if (ImGui::Button("Calculate focus")) {
				focus = lense.calculateFocus();
				for (int i = 0; i < outRays.size(); i++) {
					Plane focusPlane({ focus,0,0,1 }, {1,0,0,0});
					outRays(i).setT(focusPlane.intersectionPoint(outRays(i)));
					outRays.init();
				}
			}

			if (ImGui::Button("Scatter Plot")) {
				outRays.scatterPlot("ScatterPlot", focus);
			}

			ImGui::Text("Focus : %.5f ", focus);				
			ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		} 
		// Calclate Lense
		{
			ImGui::Begin("Calculate Lense");
			static float a1_lb{ 0.2f }, a1_rb{ 0.6f }, b1_lb{ 1.2f }, b1_rb{ 1.8f }, 
									 a2_lb{ 0.2f }, a2_rb{ 0.6f }, b2_lb{ 1.2f }, b2_rb{ 1.8f }, 
									 d_lb{ 0.1f }, d_rb{ 0.4f }, f{ 1.486f };
			ImGui::InputFloat("Focus", &f);
			ImGui::InputFloat("Left Border a1", &a1_lb);
			ImGui::InputFloat("Right Border a1", &a1_rb);
			ImGui::InputFloat("Left Border b1", &b1_lb);
			ImGui::InputFloat("Right Border b1", &b1_rb);
			ImGui::InputFloat("Left Border a2", &a2_lb);
			ImGui::InputFloat("Right Border a2", &a2_rb);
			ImGui::InputFloat("Left Border b2", &b2_lb);
			ImGui::InputFloat("Right Border b2", &b2_rb);
			ImGui::InputFloat("Left Border d", &d_lb);
			ImGui::InputFloat("Right Border d", &d_rb);

			if (ImGui::Button("Calculate")) {
				lense.calculateLense(f, a1_lb, a1_rb, b1_lb, b1_rb, a2_lb, a2_rb, b2_lb, b2_rb, d_lb, d_rb);
				inRays.clear();
				outRays.clear();
				lense.refraction(rays, inRays, outRays);
				for (int i = 0; i < outRays.size(); i++) {
					outRays(i).setT(10);
				}
				rays.init();
				inRays.init();
				outRays.init();
				lense.init();

				a1 = lense.getA1();
				b1 = lense.getB1();
				a2 = lense.getA2();
				b2 = lense.getB2();
				d = lense.getDistance();
			}
			ImGui::End();
		}
		// Rendering
		ImGui::Render();

		GLfloat currentFrame = glfwGetTime();

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		do_movement();

		shader.bind();

		glm::mat4 proj = glm::perspective(45.f, WIDTH / (float)HEIGHT, 0.1f, 100.f);
		glm::mat4 view = glm::mat4(1.f);
		view = glm::translate(view, glm::vec3(0.f, 0.f, -3.f));
		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		glm::mat4 model = glm::mat4(1.f);
		
		shader.setUniform4f("u_color", 0.3f, 0.83f, 0.99f, 0.1f);
		shader.setUniformMatrix4fv("model", model);
		shader.setUniformMatrix4fv("view", view);
		shader.setUniformMatrix4fv("projection", proj);
		
		lense.draw();

		shader.setUniform4f("u_color", 0, 1, 0, 1);
		rays.draw();

		shader.setUniform4f("u_color", 1, 0, 0, 1);
		inRays.draw();

		shader.setUniform4f("u_color", 1, 0, 0, 1);
		ruler.draw();

		shader.setUniform4f("u_color", 0, 0, 1, 1);
		outRays.draw();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	shader.unBind();
	glfwTerminate();
	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key >= 0 && key < 1024 && interactive)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
}

float calculateFocus(RayArray& rays, Lense& lense) {
	Plane plane({ lense.getDistance1(), 0, 0, 1 }, {1, 0, 0,0});

	double currentMSE{};
	double lastMSE = std::numeric_limits<double>::max();
	double step = lense.getDistance() / 100;
	double currentStep = lense.getDistance2();
	double t{};
	double li{};
	double minMSE = std::numeric_limits<double>::max();
	
	for ( ;currentStep < 10 ; currentStep += step) {
		plane.setR0({ currentStep, 0, 0, 1 });
		for (int i = 0; i < rays.size(); i++) {
			t = plane.intersectionPoint(rays(i));
			rays(i).setT(t);
		}

		for (int i = 0; i < rays.size(); i++) {
			currentMSE += std::sqrt((rays(i).r().y * rays(i).r().y + rays(i).r().z * rays(i).r().z ) / rays.size());
		}
		if (lastMSE < currentMSE) {
			plane.setR0({ currentStep - step, 0, 0, 1 });
			for (int i = 0; i < rays.size(); i++) {
				t = plane.intersectionPoint(rays(i));
				rays(i).setT(t);
			}
			return currentStep - step;
		}
		lastMSE = currentMSE;
		currentMSE = 0;
	}
	//std::cout << minMSE;
	//return minMSE;
}

void calculateLens(float focus) {

}


void do_movement()
{
	GLfloat cameraSpeed = 5.0f * deltaTime;
	if (keys[GLFW_KEY_W])
		cameraPos += cameraSpeed * cameraFront;
	if (keys[GLFW_KEY_S])
		cameraPos -= cameraSpeed * cameraFront;
	if (keys[GLFW_KEY_A])
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (keys[GLFW_KEY_D])
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

bool firstMouse = true;
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && interactive) {
		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos;
		lastX = xpos;
		lastY = ypos;

		float sensitivity = 0.4;
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		yaw += xoffset;
		pitch += yoffset;

		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;

		glm::vec3 front;
		front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		front.y = sin(glm::radians(pitch));
		front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		cameraFront = glm::normalize(front);
	}
}