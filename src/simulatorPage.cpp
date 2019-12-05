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
	getError(__LINE__);
	this->shaders.setFragmentShader("shaders/basicFragmentShader.glsl");
	getError(__LINE__);
	this->shaders.compile();
	getError(__LINE__);
}

void SimulatorPage::getError(int a_line)
{
	GLenum m_error = glGetError();
	if (m_error != GL_NO_ERROR)
		std::cout << m_error << " At line: " << a_line << std::endl;
	/*else
		std::cout << "No error at: " << a_line << std::endl;*/
}

SimulatorPage::SimulatorPage(GLFWwindow* a_window, Shader a_shader) : Page(a_window)
{
	this->shaders = a_shader;
}

void SimulatorPage::InitOpenGL()
{
	// Initializes OpenGL
	glm::vec3 m_vertices[] = {
		// Triangle 1
		glm::vec3(-0.5f, 0.5f, 0.0f), // Index 0, Top left
		glm::vec3(-0.5f, -0.5f, 0.0f), // Index 1, Bottom left
		glm::vec3(0.5f, -0.5f, 0.0f), // Index 2, Bottom right

		// Triangle 2
		glm::vec3(0.5f, 0.5f, 0.0f)  // Index 3, Top right
	};

	unsigned int m_indices[] = {
		// Triangle 1
		0, 1, 2,

		// Triangle 2
		0, 3, 2
	};
	getError(__LINE__);
	this->colorUniform = this->shaders.getUniformLocation("u_Color");
	getError(__LINE__);
	//glUniform4f(this->colorUniform, (float)this->lineColor.x, this->lineColor.y, this->lineColor.z, 1.0f);
	glUniform4f(this->colorUniform, 0.0f, 1.0f, 0.0f, 1.0f); // this->lineColor.x, this->lineColor.y, this->lineColor.z, 1.0f);
	getError(__LINE__);

	// Update the grid lines
	//this->Update(this->screenWidth, this->screenHeight);

	// Create a VAO and a VBO
	glGenVertexArrays(1, &this->vaoBuffer);
	getError(__LINE__);
	glBindVertexArray(this->vaoBuffer);
	getError(__LINE__);

	glGenBuffers(1, &this->vboBuffer);
	getError(__LINE__);
	glBindBuffer(GL_ARRAY_BUFFER, this->vboBuffer);
	getError(__LINE__);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(glm::vec3), m_vertices, GL_STATIC_DRAW);
	getError(__LINE__);


	unsigned int eboBuffer;
	glGenBuffers(1, &eboBuffer);
	getError(__LINE__);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboBuffer);
	getError(__LINE__);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), m_indices, GL_STATIC_DRAW);
	getError(__LINE__);


	glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, sizeof(glm::vec3), (void*)0);
	getError(__LINE__);
	glEnableVertexAttribArray(0);
	getError(__LINE__);

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

	getError(__LINE__);
	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	getError(__LINE__);
	
	//glBindBuffer(GL_ARRAY_BUFFER, this->vboBuffer);

	this->shaders.use();
	getError(__LINE__);

	glUniform4f(this->colorUniform, 0.0f, 1.0f, 0.0f, 1.0f); // this->lineColor.x, this->lineColor.y, this->lineColor.z, 1.0f);
	glBindVertexArray(this->vaoBuffer);
	getError(__LINE__);
	
	// Update the projection matrix with the scales
	float scalar = (600 / this->screenHeight) != 0 ? (600 / this->screenHeight) : (screenHeight / 600);	
	projectionMatrix = glm::ortho(0.0f, screenWidth * scalar, 0.0f, screenHeight * scalar, -1.0f, 1.0f);
	int matrixUniform = this->shaders.getUniformLocation("u_Projection");
	getError(__LINE__);
	glUniformMatrix4fv(matrixUniform, 1, GL_FALSE, &projectionMatrix[0][0]);
	
	//glDrawArraysInstanced(GL_LINES, 0, this->lineAmount, this->lineAmount);
	glDrawArrays(GL_TRIANGLES, 0, 0);
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
	getError(__LINE__);
	this->InitOpenGL();
	getError(__LINE__);
	bool m_exit = false;
	Page* m_nextPage = nullptr;
	while (!m_exit && !glfwWindowShouldClose(this->window))
	{
	getError(__LINE__);
		glfwPollEvents();
		this->RenderOpenGL();
		glfwSwapBuffers(this->window);
	getError(__LINE__);

		// If the escape key gets pressed, close the window.
		if (glfwGetKey(this->window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(this->window, true);

	}
	return m_nextPage;
}

