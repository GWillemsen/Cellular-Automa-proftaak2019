#include <iostream>
#include <GLFW\glfw3.h>
#include <GLM/ext/matrix_projection.hpp>
#include <glm/ext/matrix_clip_space.hpp> // glm::perspective

#include "page.h"
#include "cell.h"
#include "shader.h"

#ifndef __SIMULATORPAGE__
#define __SIMULATORPAGE__

class SimulatorPage : public Page
{
public:
	SimulatorPage(GLFWwindow* a_window);
	SimulatorPage(GLFWwindow* a_window, Shader a_shader);

private:
	Cell worldCells[1];
	int colSize = 32;

	glm::vec2* lines = nullptr;
	glm::vec3 lineColor = glm::vec3(0.4f, 0.4f, 0.4f);
	int lineAmount = 0;
	int colorUniform;
	glm::mat4 projectionMatrix;

	unsigned int vaoBuffer;
	unsigned int vboBuffer;

	Shader shaders;

public:
	Page *Run();

private:
	void InitOpenGL();
	void InitImGui();
	void InitSimulator();
	void RenderOpenGL();
	void RenderImGui();
	void UpdateSimulation();
	void DisposeOpenGL();
	void DisposeImGui();
};

#endif
