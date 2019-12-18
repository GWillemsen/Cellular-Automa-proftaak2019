// Standard system libraries
#include <thread>
#include <iostream>
#include <map>

// Dear ImGui
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

// GLAD, GLM and GLFW
#include <glad\glad.h>
#include <glm\glm.hpp>
#include <GLFW\glfw3.h>
#include <glm\gtx\transform.hpp>

// Class header files
#include "simulatorPage.h"
#include "config.h"

// Cell variables
float cellHeight = 0.1f;
float cellWidth = 0.1f;
int lastCellState = -1;

float scrollSpeed = 1.0f;
float scrollSensitivity = 1.2f;
float scrollDelayBuffer = 0.0f;

// Input variables
bool mouseIsInGUI = false;

bool leftMouseIsDown = false;
bool rightMouseIsDown = false;
bool middleMouseIsDown = false;

// Gui Variables
bool mainMenuItemFileClicked = false;

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
std::map<std::pair<GLint64, GLint64>, CellState> debugCellLocations;

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
	
	// Setup ImGUI Context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	this->imguiIO = ImGui::GetIO(); (void)this->imguiIO;
	
	// Enable keyboard input
	this->imguiIO.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

	ImGui::StyleColorsDark();

	// Setup renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(this->window, true);
	ImGui_ImplOpenGL3_Init(this->glsl_version);
}

void SimulatorPage::InitSimulator()
{
	// Initializes simulator
}

void SimulatorPage::RenderOpenGL()
{
	// Renders graphics through OpenGL

	// Render all of the cells
	this->RenderCells();

	// Render the grid as the last item, this way the grid becomes an overlay
	this->RenderGrid();
}

int selectedBrushType = 0;

static const char* cellBrushItems[] = {
	"Auto",
	"Conductor",
	"Head",
	"Tail",
	"Background",
};

void SimulatorPage::RenderImGui()
{
	// Renders the GUI with Dear ImGUI

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	{
		ImGui::BeginMainMenuBar();
		ImGui::MenuItem("File", NULL, &mainMenuItemFileClicked);
		ImGui::EndMainMenuBar();
	}

	// Create a debug frame
	{
		ImGui::Begin("Cell Brushes");
		ImGui::ListBox("Cell Brushes", &selectedBrushType, cellBrushItems, 5);
		ImGui::End();
	}

	ImGui::Render();

	ImGui::EndFrame();

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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
	this->InitImGui();

	bool m_exit = false;
	Page* m_nextPage = nullptr;
	
	while (!m_exit && !glfwWindowShouldClose(this->window))
	{
		glfwPollEvents();

		// Clear the screen
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		this->HandleInput(this->window);
		this->RenderOpenGL();
		this->CheckGuiActions();
		this->RenderImGui();

		glfwSwapBuffers(this->window);
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

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
	double m_extraY = (a_posY / this->cellSizeDivisor);

	int m_cellX = a_posX / this->cellSizeDivisor;
	int m_cellY = (a_posY - m_extraY) / this->cellSizeDivisor;

	// Update the current hovered grid cols
	if (this->curColHoveredX != m_cellX)
		this->curColHoveredX = m_cellX + this->cellScrollOffsetX;

	if (this->curColHoveredY != m_cellY)
		this->curColHoveredY = m_cellY + this->cellScrollOffsetY;

	static int m_lastCellX = 0;
	static int m_lastCellY = 0;

	bool m_equal = m_lastCellX == this->curColHoveredX && m_lastCellY == this->curColHoveredY;

	// Add cells or change cell state by dragging
	if (leftMouseIsDown && !m_equal && !rightMouseIsDown)
	{
		m_lastCellX = this->curColHoveredX;
		m_lastCellY = this->curColHoveredY;
		this->DrawGridCell(true);
	}

	static int m_lastMouseX = 0;
	static int m_lastMouseY = 0;

	// Scrolling with the middle mouse button
	if (middleMouseIsDown)
	{
		// Calculate the difference between the current and last mouse position
		int m_scrollDifferenceX = a_posX - m_lastMouseX;
		int m_scrollDifferenceY = a_posY - m_lastMouseY;

		scrollDelayBuffer += 0.1f;

		if (scrollDelayBuffer >= scrollSensitivity)
		{
			// Horizontal scrolling
			if (m_scrollDifferenceX < 0)
			{
				// Scroll right
				this->cellScrollOffsetX += scrollSpeed;
				this->gridLineScrollOffsetX -= scrollSpeed;
			}
			else if (m_scrollDifferenceX > 0)
			{
				scrollDelayBuffer += 0.1f;

				// Scroll left
				this->cellScrollOffsetX -= scrollSpeed;
				this->gridLineScrollOffsetX += scrollSpeed;
			}

			// Vertical scrolling
			if (m_scrollDifferenceY < 0)
			{
				// Scroll right
				this->cellScrollOffsetY += scrollSpeed;
				this->gridLineScrollOffsetY -= scrollSpeed;
			}
			else if (m_scrollDifferenceY > 0)
			{
				// Scroll left
				this->cellScrollOffsetY -= scrollSpeed;
				this->gridLineScrollOffsetY += scrollSpeed;
			}

			scrollDelayBuffer = 0.0f;
		}

		m_lastMouseX = a_posX;
		m_lastMouseY = a_posY;
	}

	// Remove cells by dragging
	if (rightMouseIsDown && !leftMouseIsDown)
	{
		this->RemoveGridCell();
	}
}

void SimulatorPage::MouseClick(GLFWwindow* a_window, int a_button, int a_action, int a_mods)
{
	// Left mouse click (button down)
	if (a_button == 0 && a_action == 1)
	{
		leftMouseIsDown = true;
		this->DrawGridCell(false);
	}
	// Left mouse click (button up)
	else if (a_button == 0 && a_action == 0)
	{
		leftMouseIsDown = false;

		// Resets the last cell state
		lastCellState = -1;
	}

	// Right mouse click (button down)
	if (a_button == 1 && a_action == 1)
	{
		rightMouseIsDown = true;
		this->RemoveGridCell();
	
		lastCellState = -1;
	}
	// Right mouse click (button up)
	else if (a_button == 1 && a_action == 0)
	{
		rightMouseIsDown = false;
		scrollDelayBuffer = 0.0f;

		lastCellState = -1;
	}

	// Middle mouse click (scroll wheel button clicked down)
	if (a_button == 2 && a_action == 1)
	{
		middleMouseIsDown = true;

		lastCellState = -1;
	}
	// Middle mouse click (scroll wheel button clicked up)
	else if (a_button == 2 && a_action == 0)
	{
		middleMouseIsDown = false;

		lastCellState = -1;
	}
}

void SimulatorPage::DrawGridCell(bool a_drawSameColor)
{
	auto m_foundElement = debugCellLocations.find(std::make_pair(this->curColHoveredX, this->curColHoveredY));

	if (m_foundElement == debugCellLocations.end())
	{
		if (selectedBrushType == 4)
			return;

		CellState m_cellState = CellState::Conductor;

		// Check if we need to draw the default cellState
		if (lastCellState > -1 && selectedBrushType == 0)
			m_cellState = (CellState)lastCellState;
		else
			m_cellState = (CellState)(selectedBrushType - 1);

		// Insert the new cell
		debugCellLocations.insert(std::make_pair(std::make_pair(this->curColHoveredX, this->curColHoveredY), m_cellState));
	}
	else
	{
		if (a_drawSameColor && lastCellState == -1)
			lastCellState = m_foundElement->second;

		// Get the cellstate and increment it
		int m_state = m_foundElement->second;
		m_state += 1;

		// Remove the cell when it gets the background state assigned
		if (!a_drawSameColor && (m_foundElement->second == CellState::Background || m_state > 2 || selectedBrushType == 4))
		{
			debugCellLocations.erase(m_foundElement);
		}
		else if(!a_drawSameColor && selectedBrushType == 0)
		{
			// Modify the state of the cell pointer
			m_foundElement->second = (CellState)m_state;
		}
		else if (selectedBrushType == 4)
		{
			this->RemoveGridCell();
		}
		else if (selectedBrushType > 0 && selectedBrushType != 4)
		{
			m_foundElement->second = (CellState)(selectedBrushType - 1);
		}
		else if(selectedBrushType == 0) n
		{
			//if(selectedBrushType != 0 && selectedBrushType < 5)
			m_foundElement->second = (CellState)lastCellState;
		}
	}
}

void SimulatorPage::RemoveGridCell()
{
	auto m_foundElement = debugCellLocations.find(std::make_pair(this->curColHoveredX, this->curColHoveredY));
	
	if (m_foundElement != debugCellLocations.end())
	{
		debugCellLocations.erase(m_foundElement);
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

	// Loop through the cells
	for (auto m_pair : debugCellLocations)
	{
		int m_viewportWidthMax = (int)(this->screenWidth / this->cellSizeDivisor) + this->cellScrollOffsetX;
		int m_viewportHeightMax = (int)(this->screenHeight / this->cellSizeDivisor) + this->cellScrollOffsetY;

		if (m_pair.first.first >= this->cellScrollOffsetX && m_pair.first.first < m_viewportWidthMax &&
			m_pair.first.second >= this->cellScrollOffsetY && m_pair.first.second < m_viewportHeightMax
			)
		{
			Cell m_cell(m_pair.first.first, m_pair.first.second, m_pair.second);

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

void SimulatorPage::CheckGuiActions()
{
	if (mainMenuItemFileClicked)
	{
		mainMenuItemFileClicked = false;
		std::cout << "File is clicked" << std::endl;
	}

	// Check if brushes are clicked
}