#include <iostream>
#include <GLFW\glfw3.h>
#include <GLM/ext/matrix_projection.hpp>
#include <glm/ext/matrix_clip_space.hpp> // glm::perspective

#include "page.h"
#include "world.h"
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
	const char* glsl_version = "#version 330 core";

	World worldCells;

	GLuint colorUniform;
	glm::mat4 projectionMatrix = glm::ortho(0.0f, 1.0f, 0.0f, 1.0f);

	GLuint cellVaoBuffer;
	GLuint cellVboBuffer;

	Shader shaders;

	// Grid
	Shader gridLineShader;
	Shader gridCellShader;
	glm::vec3 lineColor = glm::vec3(1.5f, 1.5f, 1.5f);

	int cellSizeDivisor = 32;
	int lineThickness = 4;
	int curColHoveredX = 0;
	int curColHoveredY = 0;
	
	float cellScrollOffsetX = 0.0f;
	float cellScrollOffsetY = 0.0f;

	float gridLineScrollOffsetX = 0.0f;
	float gridLineScrollOffsetY = 0.0f;

	GLuint gridRowVAO;
	GLuint gridRowVBO;
	GLuint gridColumnVAO;
	GLuint gridColumnVBO;

	// ImGUI
	ImGuiIO imguiIO;

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
	
	// Input
	void HandleInput(GLFWwindow* a_window);
	void MouseHover(GLFWwindow* a_window, double a_posX, double a_posY);
	void MouseClick(GLFWwindow* a_window, int a_button, int a_action, int a_mods);

	// Grid
	void InitGrid();
	void RenderGrid();
	void RenderCells();
	void UpdateCellSize();
	void DrawGridCell(bool a_drawSameColor);
	void RemoveGridCell();

	// ImGui
	void CheckGuiActions();
};

#endif
