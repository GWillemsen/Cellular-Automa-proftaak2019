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
	int colSize;

	glm::vec2* lines;
	glm::vec3 lineColor;
	int lineAmount;
	GLuint colorUniform;
	glm::mat4 projectionMatrix;

	GLuint vaoBuffer;
	GLuint vboBuffer;

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
