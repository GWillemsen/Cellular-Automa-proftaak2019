/*
MIT License

Copyright (c) 2020 Guylian Gilsing & Giel Willemsen

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

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

// Dear ImGui
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

// Dear ImGui Add-ons
#include <ImGuiFileDialog.h>

// Class header files
#include "page.h"
#include "world.h"
#include "cell.h"
#include "shader.h"
#include "config.h"

#ifndef __SIMULATORPAGE__
#define __SIMULATORPAGE__
#define InstanceBufferSize 1024*24

class SimulatorPage : public Page
{
private:
	const char* glsl_version = "#version 330 core";
	World worldCells;

	// ImGUI
	ImGuiIO* imguiIO;

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

	glm::vec2 cellOffsets[InstanceBufferSize];
	glm::vec3 cellColors[InstanceBufferSize];
	
	// OpenGL objects

	// Cell rendering
	int cellSizeInPx = 48;
	int gridLineSizeInPx = 2;

	GLuint cellVaoBuffer = -1;
	GLuint cellVboBuffer = -1;
	GLuint cellEboBuffer = -1;
	GLuint cellOffsetBuffer = -1;
	GLuint cellColorBuffer = -1;
	
	// Grid line rendering
	GLuint gridHorizontalLineVaoBuffer = -1; // Horizontal line rendering
	GLuint gridHorizontalLineVboBuffer = -1;
	
	GLuint gridVerticalLineVaoBuffer = -1; // Vertical line rendering
	GLuint gridVerticalLineVboBuffer = -1;

	// Grid system
	coordinatePart curCellHoveredX = 0;
	coordinatePart curCellHoveredY = 0;
	long int scrollDelayBuffer = 0;
	
	coordinatePart scrollOffsetX = 0;
	coordinatePart scrollOffsetY = 0;

	bool leftMouseButtonIsDown = false;
	bool rightMouseButtonIsDown = false;
	bool scrollWheelButtonIsDown = false;

	char** brushRadiusNames = new char*[6] { "1", "3", "5", "7", "9", "11" };
	int brushRadiusSelectorPos = 0;
	int brushRadius = 1;

	// The cell state that the mouse will draw in
	CellState cellDrawState = CellState::Conductor;
	char** cellDrawStateNames = new char* [4]{ "Conductor", "Head", "Tail", "Background" };;
	int selectedCellDrawName = 0;

	// GUI (Dear ImGUI)
	bool isInImguiWindow;
	bool brushWindowOpen = true;
	bool worldDetailsWindowOpen = false;
	bool pixeledView = false;
	bool debugWindowOpen = true;
	float targetSimulationSpeed = this->worldCells.GetTargetSpeed();
	bool manuallyAddKeycodesToImgui = false;
	
public:
	SimulatorPage(GLFWwindow* a_window);
	virtual Page *Run() override;

private:
	void InitOpenGL();
	void InitImGui();
	void InitSimulator();
	void RenderOpenGL();
	void RenderImGui();
	void DisposeOpenGL();
	void DisposeImGui();
	
	// Input
	virtual void MouseHover(GLFWwindow* a_window, double a_posX, double a_posY) override;
	virtual void MouseClick(GLFWwindow* a_window, int a_button, int a_action, int a_mods) override;
	virtual void MouseScroll(GLFWwindow* a_window, double a_xOffset, double a_yOffset) override;
	virtual void KeyPress(GLFWwindow* a_window, int a_key, int a_scancode, int a_action, int a_mods) override;

	// Grid
	void RenderGrid();
	void RenderCells();
	void UpdateAndRenderPendingCells(int a_pendingCellRenders);


	void AddCellToWorld(coordinatePart a_x, coordinatePart a_y);
	void RemoveCellFromWorld(coordinatePart a_x, coordinatePart a_y);
};

#endif
