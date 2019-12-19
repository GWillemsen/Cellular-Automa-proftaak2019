// Standard system libraries
#include "simulatorPage.h"

SimulatorPage::SimulatorPage(GLFWwindow* a_window) : Page(a_window, "SimulatorPage")
{
	// Create shaders
	this->gridLineShader = Shader();
	this->gridCellShader = Shader();

	// Compile the gridLine shader, this shader is going to be used to render the grid lines
	this->gridLineShader.setVertexShader("shaders/gridLineVertexShader.glsl");
	this->gridLineShader.setFragmentShader("shaders/basicFragmentShader.glsl");
	this->gridLineShader.compile();

	// Compile the gridCell shader, this shader is going to be used to render the grid cells
	this->gridCellShader.setVertexShader("shaders/gridCellVertexShader.glsl");
	this->gridCellShader.setFragmentShader("shaders/basicFragmentShader.glsl");
	this->gridCellShader.compile();
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

		// Logic
		this->HandleInput(this->window);
		this->RenderOpenGL();

		glfwSwapBuffers(this->window);
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	return m_nextPage;
}

// Initializes and presets all systems that render with OpenGL
void SimulatorPage::InitOpenGL()
{
	// Rewrite the coordinate system to be in a pixel to pixel ratio, 
	// this will make setting up the grid system more easy
	this->projectionMatrix = glm::ortho(0.0f, (float)this->screenWidth, (float)this->screenHeight, 0.0f);

	// Initialize cell buffers
	glGenVertexArrays(1, &this->cellVaoBuffer);
	glGenBuffers(1, &this->cellVboBuffer);
	glGenBuffers(1, &this->cellEboBuffer);

	glBindVertexArray(this->cellVaoBuffer);
	
	// Fill the buffers with data
	glBindBuffer(GL_ARRAY_BUFFER, this->cellVboBuffer);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(glm::vec2), this->cellVertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->cellEboBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), this->cellIndices, GL_STATIC_DRAW);

	// Define the data layout for the GPU
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
	glEnableVertexAttribArray(0);

	// Initialize grid line buffers
	glGenVertexArrays(1, &this->gridHorizontalLineVaoBuffer);
	glGenVertexArrays(1, &this->gridVerticalLineVaoBuffer);

	glGenBuffers(1, &this->gridHorizontalLineVboBuffer);
	glGenBuffers(1, &this->gridVerticalLineVboBuffer);

	// Horizontal lines
	glBindVertexArray(this->gridHorizontalLineVaoBuffer);
	
	// Fill the buffer with data
	glBindBuffer(GL_ARRAY_BUFFER, this->gridHorizontalLineVboBuffer);
	glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(glm::vec2), this->gridHorizontalLines, GL_STATIC_DRAW);

	// Define the data layout for the GPU
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
	glEnableVertexAttribArray(0);

	// Vertical lines
	glBindVertexArray(this->gridVerticalLineVaoBuffer);

	// Fill the buffer with data
	glBindBuffer(GL_ARRAY_BUFFER, this->gridVerticalLineVboBuffer);
	glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(glm::vec2), this->gridVerticalLines, GL_STATIC_DRAW);

	// Define the data layout for the GPU
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
	glEnableVertexAttribArray(0);

	//this->InitGrid();
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
	
	// Render all the cells within the viewport
	this->RenderCells();

	// Render the grid lines
	this->RenderGrid();
}

void SimulatorPage::HandleInput(GLFWwindow* a_window)
{
	if (glfwGetKey(a_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(this->window, true);

	if (glfwGetKey(a_window, GLFW_KEY_S) == GLFW_PRESS)
		this->worldCells.StartSimulation();

	if (glfwGetKey(a_window, GLFW_KEY_P) == GLFW_PRESS)
		this->worldCells.PauzeSimulation();

	if (glfwGetKey(a_window, GLFW_KEY_1) == GLFW_PRESS)
	{
		float m_speed = this->worldCells.GetTargetSpeed();
		this->worldCells.SetTargetSpeed(m_speed + 2.0f);
	}
	if (glfwGetKey(a_window, GLFW_KEY_0) == GLFW_PRESS)
	{
		float m_speed = this->worldCells.GetTargetSpeed();
		this->worldCells.SetTargetSpeed(m_speed - 2.0f);
	}
}

void SimulatorPage::MouseHover(GLFWwindow* a_window, double a_posX, double a_posY)
{
	// Calculate in which cell the mouse cursor is located
	long m_curCellXHovered = ((long)(this->gridLineSizeInPx + a_posX) / this->cellSizeInPx);
	long m_curCellYHovered = ((long)(this->gridLineSizeInPx + a_posY) / this->cellSizeInPx);

	if (m_curCellXHovered != this->curCellHoveredX)
		this->curCellHoveredX = m_curCellXHovered;

	if (m_curCellYHovered != this->curCellHoveredY)
		this->curCellHoveredY = m_curCellYHovered;

	static int m_lastCellX = 0;
	static int m_lastCellY = 0;

	// Add / remove cells while the mouse is being dragged around
	if (this->leftMouseButtonIsDown)
	{
		if (m_lastCellX != m_curCellXHovered || m_lastCellY != m_curCellYHovered)
		{
			this->AddCellToWorld(true);

			m_lastCellX = m_curCellXHovered;
			m_lastCellY = m_curCellYHovered;
		}
	}
	else if (this->rightMouseButtonIsDown)
	{
		if (m_lastCellX != m_curCellXHovered || m_lastCellY != m_curCellYHovered)
		{
			this->RemoveCellFromWorld();

			m_lastCellX = m_curCellXHovered;
			m_lastCellY = m_curCellYHovered;
		}
	}
}

void SimulatorPage::MouseClick(GLFWwindow* a_window, int a_button, int a_action, int a_mods)
{
	// Left mouse button press
	if (a_button == 0 && a_action == 1)
	{
		this->leftMouseButtonIsDown = true;
		this->AddCellToWorld(false);
	}
	// Left mouse button release
	else if (a_button == 0 && a_action == 0)
	{
		this->leftMouseButtonIsDown = false;
	}

	// Right mouse button press
	if (a_button == 1 && a_action == 1)
	{
		this->rightMouseButtonIsDown = true;
		this->RemoveCellFromWorld();
	}
	// Right mouse button release
	else if (a_button == 1 && a_action == 0)
	{
		this->rightMouseButtonIsDown = false;
	}
}

void SimulatorPage::RenderCells()
{
	// Set the projection matrix
	this->gridCellShader.use();
	int m_projectionMatrixUniform = this->gridCellShader.getUniformLocation("u_ProjectionMatrix");

	glUniformMatrix4fv(m_projectionMatrixUniform, 1, GL_FALSE, &this->projectionMatrix[0][0]);

	// Render all of the worldcells
	for (auto m_worldCell : this->worldCells.cells)
	{
		m_worldCell.second->Render(this->cellSizeInPx);
	}
}

void SimulatorPage::RenderGrid()
{
	this->gridLineShader.use();
	
	// Get the uniforms
	int m_projectionMatrixUniform = this->gridLineShader.getUniformLocation("u_ProjectionMatrix");
	int m_drawHorizontalUniform = this->gridLineShader.getUniformLocation("u_DrawHorizontal");
	int m_cellSizeInPxUniform = this->gridLineShader.getUniformLocation("u_CellSizeInPx");
	int m_colorUniform = this->gridLineShader.getUniformLocation("u_Color");

	// Set the grid line size
	glLineWidth((GLfloat)this->gridLineSizeInPx);

	// Set the projection matrix
	glUniformMatrix4fv(m_projectionMatrixUniform, 1, GL_FALSE, &this->projectionMatrix[0][0]);

	// Set the line color
	glUniform4f(m_colorUniform, 0.5f, 0.5f, 0.5f, 1.0f);

	// Set the cell size in pixels
	glUniform1i(m_cellSizeInPxUniform, this->cellSizeInPx);

	// Draw horizontal grid lines
	glUniform1i(m_drawHorizontalUniform, 1);

	int m_lineCount = this->screenHeight / this->cellSizeInPx;

	glBindVertexArray(this->gridHorizontalLineVaoBuffer);

	glDrawArraysInstanced(GL_LINES, 0, 2, m_lineCount + 2);

	// Draw vertical grid lines
	glUniform1i(m_drawHorizontalUniform, 0);

	glBindVertexArray(this->gridVerticalLineVaoBuffer);

	m_lineCount = this->screenWidth / this->cellSizeInPx;

	glBindVertexArray(this->gridVerticalLineVaoBuffer);

	glDrawArraysInstanced(GL_LINES, 0, 2, m_lineCount + 2);
}

void SimulatorPage::AddCellToWorld(bool a_mouseIsBeingDragged)
{
	auto m_worldCell = this->worldCells.cells.find(std::make_pair(this->curCellHoveredX, this->curCellHoveredY));

	CellState m_cellState = CellState::Conductor; // Gets manipulated based on logic

	// Insert a new cell
	if (m_worldCell == this->worldCells.cells.end())
	{
		// No cells found, insert the new cell into the world
		this->worldCells.InsertCellAt(this->curCellHoveredX, this->curCellHoveredY, m_cellState);

		// Find the newly added worldcell and initialize the rendering
		auto m_newWorldCell = this->worldCells.cells.find(std::make_pair(this->curCellHoveredX, this->curCellHoveredY));
		
		if (m_newWorldCell != this->worldCells.cells.end())
			m_newWorldCell->second->InitRender(this->gridCellShader, this->cellVaoBuffer);
	}
	// Change cell state
	else
	{
		// Only cycle through the state by clicking
		if (!a_mouseIsBeingDragged)
		{
			// Update to the new cell state
			if (m_cellState != m_worldCell->second->cellState)
				m_cellState = m_worldCell->second->cellState;

			if (((CellState)(int)m_cellState + 1) != CellState::Background)
				m_cellState = (CellState)((int)m_cellState + 1);
			// Reset the cellstate back to Conductor when it is being set to the background state
			else
				m_cellState = CellState::Conductor;
		}

		m_worldCell->second->cellState = m_cellState;
	}
}

void SimulatorPage::RemoveCellFromWorld()
{
	auto m_worldCell = this->worldCells.cells.find(std::make_pair(this->curCellHoveredX, this->curCellHoveredY));

	// Make sure that the cell exists within the world
	if (m_worldCell != this->worldCells.cells.end())
	{
		this->worldCells.cells.erase(m_worldCell);
	}
}