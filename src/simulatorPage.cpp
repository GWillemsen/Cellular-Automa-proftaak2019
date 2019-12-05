#include <thread>
#include <iostream>
#include <glad\glad.h>
#include <glm\glm.hpp>
#include <GLFW\glfw3.h>
#include "simulatorPage.h"
#include "shader.h"
#include "config.h"

SimulatorPage::SimulatorPage(GLFWwindow* a_window) : Page(a_window)
{
	this->shaders = Shader();

	this->shaders.setVertexShader("shaders/basicVertexShader.glsl");
	this->shaders.setFragmentShader("shaders/basicFragmentShader.glsl");
	this->shaders.compile();
}

SimulatorPage::SimulatorPage(GLFWwindow* a_window, Shader a_shader) : Page(a_window)
{
	this->shaders = a_shader;
}

void SimulatorPage::InitOpenGL()
{
	// Initializes OpenGL

	this->colorUniform = this->shaders.getUniformLocation("u_Color");
	glUniform4f(this->colorUniform, this->lineColor.x, this->lineColor.y, this->lineColor.z, 1.0f);

	// Update the grid lines
	//this->Update(this->screenWidth, this->screenHeight);

	// Create a VAO and a VBO
	glGenVertexArrays(1, &this->vaoBuffer);
	glBindVertexArray(this->vaoBuffer);

	glGenBuffers(1, &this->vboBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, this->vboBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * (this->lineAmount / 2), this->lines, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
	glEnableVertexAttribArray(0);


	// Calculate the amount of lines the X and Y dimensions need.
	// We add 2 extra lines on each dimension in order to create a smooth scrolling effect.
	int m_horizontalLinesAmount = (this->screenWidth / this->colSize) + 2;
	int m_verticalLinesAmount = (this->screenHeight / this->colSize) + 2;

	// Multiply the total line amount since each line has 2 vertices
	const int totalLineAmount = (m_horizontalLinesAmount + m_verticalLinesAmount) * 2;
	this->lineAmount = totalLineAmount;

	if (this->lines)
		delete[] this->lines;
	// Update the lines array.
	this->lines = new glm::vec2[totalLineAmount];

	for (int i = 0; i < m_horizontalLinesAmount; i += 2)
	{
		this->lines[i] = glm::vec2(this->colSize * i, 0.0);

		// Make sure that the loop does not get out of bounds
		if (i < totalLineAmount)
			this->lines[i + 1] = glm::vec2(this->colSize * i, (float)(this->colSize * m_horizontalLinesAmount));
	}

	int m_loopOffset = m_horizontalLinesAmount + 1;

	for (int i = 1; i < m_verticalLinesAmount + 1; i += 2)
	{
		this->lines[m_loopOffset + i] = glm::vec2(0.0f, this->colSize * i);

		// Make sure that the loop does not get out of bounds
		if ((m_loopOffset + i) < totalLineAmount)
			this->lines[m_loopOffset + i + 1] = glm::vec2((float)(this->colSize * m_verticalLinesAmount), this->colSize * i);
	}
}

void SimulatorPage::InitImGui()
{
	// Initializes Dear ImGui
}

void SimulatorPage::InitSimulator()
{
	// Initializes simulator
}

void SimulatorPage::RenderOpenGL()
{
	// Renders graphics through OpenGL
	this->shaders.use();

	glBindVertexArray(this->vaoBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, this->vboBuffer);

	glClearColor(1, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	glUniform4f(this->colorUniform, this->lineColor.x, this->lineColor.y, this->lineColor.z, 1.0f);
	//glDrawArraysInstanced(GL_LINES, 0, this->lineAmount, this->lineAmount);


	float scalar = (600 / this->screenHeight) != 0 ? (600 / this->screenHeight) : (screenHeight / 600);
	
	// Update the projection matrix and apply the scales
	projectionMatrix = glm::ortho(0.0f, screenWidth * scalar, 0.0f, screenHeight * scalar, -1.0f, 1.0f);

	int matrixUniform = this->shaders.getUniformLocation("u_Projection");
	glUniformMatrix4fv(matrixUniform, 1, GL_FALSE, &projectionMatrix[0][0]);
}

void SimulatorPage::RenderImGui()
{
	// Renders the GUI with Dear ImGUI
}

void SimulatorPage::UpdateSimulation()
{
	// Updates the simulation
}

void SimulatorPage::DisposeOpenGL()
{
	// Removes all OpenGL pointers, etc.
}

void SimulatorPage::DisposeImGui()
{
	// Removes Dear ImGUI
}

Page* SimulatorPage::Run()
{
	this->InitOpenGL();
	bool m_exit = false;
	Page* m_nextPage = nullptr;
	while (!m_exit && !glfwWindowShouldClose(this->window))
	{
		glfwPollEvents();
		this->RenderOpenGL();
		glfwSwapBuffers(this->window);

		// If the escape key gets pressed, close the window.
		if (glfwGetKey(this->window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(this->window, true);

	}
	return m_nextPage;
}

