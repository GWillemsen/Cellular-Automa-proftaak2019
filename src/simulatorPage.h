// System libaries
#include <iostream>
#include <thread>
#include <map>

// GLAD, GLM and GLFW
#include <glad\glad.h>
#include <GLFW\glfw3.h>

#include <glm\glm.hpp>
#include <glm\gtx\transform.hpp>
#include <glm\ext\matrix_projection.hpp>
#include <glm\ext\matrix_clip_space.hpp> // glm::perspective

// Dear ImGui
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

// Class header files
#include "page.h"
#include "world.h"
#include "cell.h"
#include "shader.h"
#include "config.h"

#ifndef __SIMULATORPAGE__
#define __SIMULATORPAGE__

class SimulatorPage : public Page
{
private:
	const char* glsl_version = "#version 330 core";
	World worldCells;

	// ImGUI
	ImGuiIO imguiIO;

	// Coordinate system
	glm::mat4 projectionMatrix = glm::ortho(0.0f, (float)this->screenWidth, (float)this->screenHeight, 0.0f);

	// Shaders
	Shader gridLineShader;
	Shader gridCellShader;

	// Vertices, Indices and Matrices
	glm::vec2 cellVertices[6] = {
		glm::vec2(0.0f, 0.0f),
		glm::vec2(0.0f, 1.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(1.0f, 0.0f),
	};

	unsigned int cellIndices[6] = {
		0, 1, 2,
		0, 3, 2
	};

	glm::vec2 gridHorizontalLines[2] = {
		glm::vec2(0.0f, 0.0f),
		glm::vec2(this->screenWidth, 0.0f),
	};

	glm::vec2 gridVerticalLines[2] = {
		glm::vec2(0.0f, 0.0f),
		glm::vec2(0.0f, this->screenHeight),
	};

	// OpenGL objects

	// Cell rendering
	int cellSizeInPx = 48;
	int gridLineSizeInPx = 2;

	GLuint cellVaoBuffer = -1;
	GLuint cellVboBuffer = -1;
	GLuint cellEboBuffer = -1;

	// Grid line rendering
	GLuint gridHorizontalLineVaoBuffer = -1; // Horizontal line rendering
	GLuint gridHorizontalLineVboBuffer = -1;
	
	GLuint gridVerticalLineVaoBuffer = -1; // Vertical line rendering
	GLuint gridVerticalLineVboBuffer = -1;

	// Grid system
	long curCellHoveredX = 0;
	long curCellHoveredY = 0;

public:
	SimulatorPage(GLFWwindow* a_window);
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
	void RenderGrid();
	void RenderCells();

	// ImGui
	void CheckGuiActions();
};

#endif
