#include <thread>
#include <iostream>
#include <map>
#include <glad\glad.h>
#include <glm\glm.hpp>
#include <GLFW\glfw3.h>
#include "simulatorPage.h"
#include "shader.h"
#include "config.h"
#include <glm\gtx\transform.hpp>

// Cell variables
float cellHeight = 0.1f;
float cellWidth = 0.1f;

glm::vec3 cellVertices[] = {
	// Triangle 1
	glm::vec3(0.0f, 0.0f, 0.0f), // Index 0, Top left
	glm::vec3(0.0f, cellHeight, 0.0f), // Index 1, Bottom left
	glm::vec3(cellWidth, cellHeight, 0.0f), // Index 2, Bottom right

	// Triangle 2
	glm::vec3(cellWidth, 0.0f, 0.0f)  // Index 3, Top right
};

unsigned int cellIndices[] = {
	// Triangle 1
	0, 1, 2,

	// Triangle 2
	0, 3, 2
};

// Cell debug variables
std::map<std::pair<int, int>, CellState> debugCellLocations;

SimulatorPage::SimulatorPage(GLFWwindow* a_window) : Page(a_window, "SimulatorPage")
{
	this->shaders = Shader();

	this->shaders.setVertexShader("shaders/basicVertexShader.glsl");
	this->shaders.setFragmentShader("shaders/basicFragmentShader.glsl");
	this->shaders.compile();

	this->lineColor = glm::vec3(0.4f, 0.4f, 0.4f);
	this->colorUniform = 0;

	this->cellVaoBuffer = 0;
	this->cellVboBuffer = 0;
}

SimulatorPage::SimulatorPage(GLFWwindow* a_window, Shader a_shader) : SimulatorPage(a_window)
{
	this->shaders = a_shader;
}

void SimulatorPage::InitOpenGL()
{
	// Set some uniform variable in the default shader
	this->shaders.use();
	this->colorUniform = this->shaders.getUniformLocation("u_Color");
	this->projectionMatrix = glm::ortho(0.0f, 1.0f, 1.0f, 0.0f);

	glUniform4f(this->colorUniform, this->lineColor.x, this->lineColor.y, this->lineColor.z, 1.0f);

	// Create a VAO and a VBO
	glGenVertexArrays(1, &this->cellVaoBuffer);
	glBindVertexArray(this->cellVaoBuffer);

	// Fill the VBO with data
	glGenBuffers(1, &this->cellVboBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, this->cellVboBuffer);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(glm::vec3), cellVertices, GL_DYNAMIC_DRAW);

	// Create an EBO and fill it with data
	unsigned int eboBuffer;
	glGenBuffers(1, &eboBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), cellIndices, GL_DYNAMIC_DRAW);

	// Define the data layout
	// CHANGE TO GL_TRUE WHEN BRICKED and tell Guylian that this was a mistake!
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(0);

	// Fill the cell locations
	debugCellLocations.insert(std::make_pair(std::make_pair(1, 1), CellState::Conductor));
	debugCellLocations.insert(std::make_pair(std::make_pair(2, 2), CellState::Conductor));
	debugCellLocations.insert(std::make_pair(std::make_pair(3, 3), CellState::Head));
	// Out of bounds
	debugCellLocations.insert(std::make_pair(std::make_pair(40, 1), CellState::Conductor));
	debugCellLocations.insert(std::make_pair(std::make_pair(41, 2), CellState::Conductor));
	debugCellLocations.insert(std::make_pair(std::make_pair(42, 3), CellState::Conductor));

	this->cellSizeDivisor = 48;

	this->UpdateCellSize();
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

	// Clear the screen
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// Render all of the cells
	this->RenderCells();

	// Render the grid as the last item, this way the grid becomes an overlay
	this->RenderGrid();
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

		this->HandleInput(this->window);
		this->RenderOpenGL();

		glfwSwapBuffers(this->window);
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

	// Create and compile shaders
	this->gridCellShader.setVertexShader("shaders/gridCellVertexShader.glsl");
	this->gridCellShader.setFragmentShader("shaders/basicFragmentShader.glsl");
	this->gridCellShader.compile();

	this->gridLineShader.setVertexShader("shaders/gridLineVertexShader.glsl");
	this->gridLineShader.setFragmentShader("shaders/basicFragmentShader.glsl");
	this->gridLineShader.compile();
	this->gridLineShader.use();

	glUniformMatrix4fv(gridLineShader.getUniformLocation("u_Projection"), 1, GL_FALSE, &this->projectionMatrix[0][0]);

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
}

void SimulatorPage::RenderGrid()
{
	this->gridLineShader.use();

	int colorUniform = gridLineShader.getUniformLocation("u_Color");
	int matrixUniform = gridLineShader.getUniformLocation("u_Projection");
	int colCountUniform = gridLineShader.getUniformLocation("u_ColCount");
	int colOrRowUniform = gridLineShader.getUniformLocation("u_ColOrRow");

	// Set the line color
	glUniform4f(colorUniform, this->lineColor.x, this->lineColor.y, this->lineColor.z, 1.0f);

	// Set the projection matrix
	glUniformMatrix4fv(matrixUniform, 1, GL_FALSE, &this->projectionMatrix[0][0]);

	// Set the line width to 4
	glLineWidth(this->lineThickness);

	// Draw vertical lines
	int loopAmount = (this->screenWidth / this->cellSizeDivisor);
	glUniform1i(colCountUniform, loopAmount);

	glBindVertexArray(this->gridRowVAO);
	glUniform1i(colOrRowUniform, 1); // Start rendering rows (Vertical lines
	glDrawArraysInstanced(GL_LINES, 0, GL_UNSIGNED_INT, loopAmount + 2);

	// Draw horizontal lines
	loopAmount = (this->screenHeight / this->cellSizeDivisor);
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

void SimulatorPage::MouseHover(GLFWwindow* a_window, double a_posX, double a_posY)
{
	// Calculate in which cell the mouse pointer is located
	int m_cellX = ((a_posX) / this->cellSizeDivisor);
	int m_cellY = ((a_posY) / this->cellSizeDivisor);

	// Update the current hovered grid cols
	if (this->curColHoveredX != m_cellX)
		this->curColHoveredX = m_cellX + this->cellScrollOffsetX;

	if (this->curColHoveredY != m_cellY)
		this->curColHoveredY = m_cellY + this->cellScrollOffsetY;
}

void SimulatorPage::MouseClick(GLFWwindow* a_window, int a_button, int a_action, int a_mods)
{
	// Left mouse click
	if (a_button == 0 && a_action == 1)
	{
		auto m_foundElement = debugCellLocations.find(std::make_pair(this->curColHoveredX, this->curColHoveredY));

		if (m_foundElement == debugCellLocations.end())
		{
			debugCellLocations.insert(std::make_pair(std::make_pair(this->curColHoveredX, this->curColHoveredY), CellState::Conductor));
		}
		else
		{
			// Cycle through the cellstates
			if(m_foundElement->second == CellState::Background)
			{
				debugCellLocations.erase(m_foundElement);
			}
			else
			{
				// Get the cellstate and increment it
				int m_state = m_foundElement->second;
				m_state++;
					
				std::cout << (CellState)m_state << std::endl;

				// Modify the state of the cell pointer
				m_foundElement->second = (CellState)m_state;
			}
		}
	}
}

// Cell rendering
void SimulatorPage::RenderCells()
{
	this->gridCellShader.use();

	// Bind the grid shader
	this->gridCellShader.use();
	this->colorUniform = gridCellShader.getUniformLocation("u_Color");

	// Sets the cell colors
	glUniform4f(this->colorUniform, 1.0f, 1.0f, 1.0f, 1.0f);
	glBindVertexArray(this->cellVaoBuffer);

	// Update the projection matrix with the scales
	int matrixUniform = this->gridCellShader.getUniformLocation("u_Projection");
	glUniformMatrix4fv(matrixUniform, 1, GL_FALSE, &this->projectionMatrix[0][0]);

	// Get the grid cell size uniforms and update them
	int cellWidthUniform = this->gridCellShader.getUniformLocation("u_CellWidth");
	int cellHeightUniform = this->gridCellShader.getUniformLocation("u_CellHeight");

	glUniform1f(cellWidthUniform, 1.0f / (this->screenWidth / this->cellSizeDivisor));
	glUniform1f(cellHeightUniform, 1.0f / (this->screenHeight / this->cellSizeDivisor));

	// Loop through the 	
	for (auto m_pair : debugCellLocations)
	{
		int m_viewportWidthMax = (int)(this->screenWidth / this->cellSizeDivisor) + this->cellScrollOffsetX;
		int m_viewportHeightMax = (int)(this->screenHeight/ this->cellSizeDivisor) + this->cellScrollOffsetY;

		if (m_pair.first.first >= this->cellScrollOffsetX && m_pair.first.first < m_viewportWidthMax &&
			m_pair.first.second >= this->cellScrollOffsetY && m_pair.first.second < m_viewportHeightMax
			)
		{
			Cell m_cell = Cell();

			m_cell.x = m_pair.first.first;
			m_cell.y = m_pair.first.second;
			m_cell.cellState = m_pair.second;

			int cellOffsetVerticalUniform = this->gridCellShader.getUniformLocation("u_VerticalOffset");
			int cellOffsetHorizontalUniform = this->gridCellShader.getUniformLocation("u_HorizontalOffset");

			glUniform1i(cellOffsetVerticalUniform, m_cell.y - this->cellScrollOffsetY);
			glUniform1i(cellOffsetHorizontalUniform, m_cell.x - this->cellScrollOffsetX);

			// Set the color of the cell
			if (m_cell.cellState == CellState::Conductor)
			{
				glUniform4f(this->colorUniform, 1.0f, 1.0f, 0.0f, 1.0f);
			}
			else if (m_cell.cellState == CellState::Head)
			{
				glUniform4f(this->colorUniform, 1.0f, 0.0f, 0.0f, 1.0f);
			}
			else if (m_cell.cellState == CellState::Tail)
			{
				glUniform4f(this->colorUniform, 0.0f, 0.0f, 1.0f, 1.0f);
			}

			m_cell.InitRender(m_cell.x, m_cell.y);
			m_cell.Render(this->colorUniform, this->cellVaoBuffer);
		}
		else
			continue;
	}
}

void SimulatorPage::UpdateCellSize()
{
	cellHeight = (1.0f / (this->screenHeight / this->cellSizeDivisor));
	cellWidth = (1.0f / (this->screenWidth / this->cellSizeDivisor));

	glm::vec3 m_newCellVertices[] = {
		// Triangle 1
		glm::vec3(0.0f, 0.0f, 0.0f), // Index 0, Top left
		glm::vec3(0.0f, cellHeight, 0.0f), // Index 1, Bottom left
		glm::vec3(cellWidth, cellHeight, 0.0f), // Index 2, Bottom right

		// Triangle 2
		glm::vec3(cellWidth, 0.0f, 0.0f)  // Index 3, Top right
	};

	glBindVertexArray(this->cellVaoBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, this->cellVboBuffer);

	// Update the VBO data
	glm::vec3* m_bufferDataPtr = (glm::vec3*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	memcpy(m_bufferDataPtr, &m_newCellVertices, sizeof(m_newCellVertices));
	glUnmapBuffer(GL_ARRAY_BUFFER);
}