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
	
	this->RenderCells();

	this->RenderGrid();
}

void SimulatorPage::HandleInput(GLFWwindow* a_window)
{
	if (glfwGetKey(a_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(this->window, true);
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
}

void SimulatorPage::MouseClick(GLFWwindow* a_window, int a_button, int a_action, int a_mods)
{
}

void SimulatorPage::RenderCells()
{
	// Set the projection matrix
	this->gridCellShader.use();
	int m_projectionMatrixUniform = this->gridCellShader.getUniformLocation("u_ProjectionMatrix");

	glUniformMatrix4fv(m_projectionMatrixUniform, 1, GL_FALSE, &this->projectionMatrix[0][0]);

	// Render cells
	Cell m_testCell(0, 0, CellState::Conductor);
	Cell m_testCell2(1, 1, CellState::Head);
	Cell m_testCell3(2, 2, CellState::Tail);

	m_testCell.InitRender(this->gridCellShader, this->cellVaoBuffer);
	m_testCell.Render(this->cellSizeInPx, this->gridLineSizeInPx);

	m_testCell2.InitRender(this->gridCellShader, this->cellVaoBuffer);
	m_testCell2.Render(this->cellSizeInPx, this->gridLineSizeInPx);

	m_testCell3.InitRender(this->gridCellShader, this->cellVaoBuffer);
	m_testCell3.Render(this->cellSizeInPx, this->gridLineSizeInPx);
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