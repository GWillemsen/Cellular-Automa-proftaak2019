#include <thread>
#include <iostream>
#include <glad\glad.h>
#include <glm\glm.hpp>
#include <GLFW\glfw3.h>
#include "simulatorPage.h"
#include "shader.h"
#include "config.h"
#include <glm\gtx\transform.hpp>

// Grid variables
int gridWidth = 800;
int gridHeight = 600;

SimulatorPage::SimulatorPage(GLFWwindow* a_window) : Page(a_window, "SimulatorPage")
{
	this->shaders = Shader();

	this->shaders.setVertexShader("shaders/basicVertexShader.glsl");
	this->GetError(__LINE__);
	this->shaders.setFragmentShader("shaders/basicFragmentShader.glsl");
	this->GetError(__LINE__);
	this->shaders.compile();
	this->GetError(__LINE__);

	this->colSize = 32;
	this->lineColor = glm::vec3(0.4f, 0.4f, 0.4f);
	this->colorUniform = 0; // glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	this->projectionMatrix = glm::mat4(1.0f);

	this->vaoBuffer = 0;
	this->vboBuffer = 0;
}

SimulatorPage::SimulatorPage(GLFWwindow* a_window, Shader a_shader) : SimulatorPage(a_window)
{
	this->shaders = a_shader;
}

void SimulatorPage::InitOpenGL()
{
	// Initializes OpenGL
	glm::vec3 m_vertices[] = {
		// Triangle 1
		glm::vec3(0.0f, 0.0f, 0.0f), // Index 0, Top left
		glm::vec3(0.0f, 0.15f, 0.0f), // Index 1, Bottom left
		glm::vec3(0.10f, 0.15f, 0.0f), // Index 2, Bottom right

		// Triangle 2
		glm::vec3(0.10f, 0.0f, 0.0f)  // Index 3, Top right
	};

	unsigned int m_indices[] = {
		// Triangle 1
		0, 1, 2,

		// Triangle 2
		0, 3, 2
	};
	this->shaders.use();
	this->GetError(__LINE__);
	this->colorUniform = this->shaders.getUniformLocation("u_Color");
	this->GetError(__LINE__);
	//glUniform4f(this->colorUniform, (float)this->lineColor.x, this->lineColor.y, this->lineColor.z, 1.0f);
	glUniform4f(this->colorUniform, 0.0f, 1.0f, 0.0f, 1.0f); // this->lineColor.x, this->lineColor.y, this->lineColor.z, 1.0f);
	this->GetError(__LINE__);
	
	// Create a VAO and a VBO
	glGenVertexArrays(1, &this->vaoBuffer);
	this->GetError(__LINE__);
	glBindVertexArray(this->vaoBuffer);
	this->GetError(__LINE__);

	glGenBuffers(1, &this->vboBuffer);
	this->GetError(__LINE__);
	glBindBuffer(GL_ARRAY_BUFFER, this->vboBuffer);
	this->GetError(__LINE__);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(glm::vec3), m_vertices, GL_STATIC_DRAW);
	this->GetError(__LINE__);


	unsigned int eboBuffer;
	glGenBuffers(1, &eboBuffer);
	this->GetError(__LINE__);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboBuffer);
	this->GetError(__LINE__);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), m_indices, GL_STATIC_DRAW);
	this->GetError(__LINE__);


	glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, sizeof(glm::vec3), (void*)0);
	this->GetError(__LINE__);
	glEnableVertexAttribArray(0);
	this->GetError(__LINE__);

	this->InitGrid();
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

	this->GetError(__LINE__);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	this->GetError(__LINE__);

	//glBindBuffer(GL_ARRAY_BUFFER, this->vboBuffer);
	this->RenderGrid();

	this->shaders.use();
	this->GetError(__LINE__);

	glUniform4f(this->colorUniform, 0.0f, 1.0f, 0.0f, 1.0f); // this->lineColor.x, this->lineColor.y, this->lineColor.z, 1.0f);
	glBindVertexArray(this->vaoBuffer);
	this->GetError(__LINE__);

	// Update the projection matrix with the scales
	this->projectionMatrix = glm::ortho(0.0f, 1.0f, 1.0f, 0.0f);
	int matrixUniform = this->shaders.getUniformLocation("u_Projection");
	this->GetError(__LINE__);
	glUniformMatrix4fv(matrixUniform, 1, GL_FALSE, &this->projectionMatrix[0][0]);

	//glDrawArraysInstanced(GL_LINES, 0, this->lineAmount, this->lineAmount);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
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
	this->GetError(__LINE__);
	this->InitOpenGL();
	this->GetError(__LINE__);
	bool m_exit = false;
	Page* m_nextPage = nullptr;
	while (!m_exit && !glfwWindowShouldClose(this->window))
	{
		this->GetError(__LINE__);
		glfwPollEvents();

		this->HandleInput(this->window);

		this->RenderOpenGL();
		this->GetError(__LINE__);
		
		glfwSwapBuffers(this->window);
		this->GetError(__LINE__);
	}
	return m_nextPage;
}

// Grid system
void SimulatorPage::InitGrid()
{
	glm::vec2 m_gridColumns[] = {
		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 0.0f)
	};

	glm::vec2 m_gridRows[] = {
		glm::vec2(0.0f, 0.0f),
		glm::vec2(0.0f, 1.0f)
	};

	// Create and compile a new shader
	this->gridShader.setVertexShader("shaders/gridVertexShader.glsl");
	this->gridShader.setFragmentShader("shaders/basicFragmentShader.glsl");
	this->gridShader.compile();

	this->gridShader.use();

	this->projectionMatrix = glm::ortho(0.0f, 1.0f, 0.0f, 1.0f);
	glUniformMatrix4fv(gridShader.getUniformLocation("u_Projection"), 1, GL_FALSE, &this->projectionMatrix[0][0]);

	// Create and bind a new VAO
	glGenVertexArrays(1, &this->gridColumnVAO);
	glBindVertexArray(this->gridColumnVAO);
	
	// Bind grid columns
	glGenBuffers(1, &this->gridColumnVBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->gridColumnVBO);
	glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(glm::vec2), m_gridColumns, GL_STATIC_DRAW);

	// Define data layout
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
	glEnableVertexAttribArray(0);

	// Create and bind a new VAO
	glGenVertexArrays(1, &this->gridRowVAO);
	glBindVertexArray(this->gridRowVAO);

	// Bind grid rows
	glGenBuffers(1, &this->gridRowVBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->gridRowVBO);
	glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(glm::vec2), m_gridRows, GL_STATIC_DRAW);

	// Define data layout
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
	glEnableVertexAttribArray(0);

	this->colSize = 64;
}

void SimulatorPage::RenderGrid()
{
	this->gridShader.use();

	int colorUniform = gridShader.getUniformLocation("u_Color");
	int matrixUniform = gridShader.getUniformLocation("u_Projection");
	int colCountUniform = gridShader.getUniformLocation("u_ColCount");
	int colOrRowUniform = gridShader.getUniformLocation("u_ColOrRow");

	// Set the line color
	glUniform4f(colorUniform, this->lineColor.x, this->lineColor.y, this->lineColor.z, 1.0f);

	// Set the line width to 4
	glLineWidth(4);

	// Draw vertical lines
	int loopAmount = (this->screenWidth / this->colSize);
	glUniform1i(colCountUniform, loopAmount);

	glBindVertexArray(this->gridRowVAO);
	glUniform1i(colOrRowUniform, 1); // Start rendering rows (Vertical lines
	glDrawArraysInstanced(GL_LINES, 0, GL_UNSIGNED_INT, loopAmount + 2);

	// Draw horizontal lines
	loopAmount = (this->screenHeight/ this->colSize);
	glUniform1i(colCountUniform, loopAmount);

	glBindVertexArray(this->gridColumnVAO);
	glUniform1i(colOrRowUniform, 0); // Start rendering columns (Horizontal lines)
	glDrawArraysInstanced(GL_LINES, 0, GL_UNSIGNED_INT, loopAmount + 2);
}

void SimulatorPage::HandleInput(GLFWwindow* a_window)
{
	// If the escape key gets pressed, close the window
	if (glfwGetKey(a_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(this->window, true);
}

void SimulatorPage::MouseHover(GLFWwindow *a_window, double a_posX, double a_posY)
{
	// Calculate in which cell the mouse pointer is located
	int m_cellX = ((a_posX) / this->colSize);
	int m_cellY = ((a_posY) / this->colSize);

	// Update the current hovered grid cols
	if (this->curColHoveredX != m_cellX)
		this->curColHoveredX = m_cellX;

	if (this->curColHoveredY!= m_cellY)
		this->curColHoveredY = m_cellY;

	std::cout << "cellX: " << m_cellX << "cellY: " << m_cellY << std::endl;
}

void SimulatorPage::MouseClick(GLFWwindow* a_window, int a_button, int a_action, int a_mods)
{

}