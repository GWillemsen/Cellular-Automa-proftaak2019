// Standard system libraries
#include "simulatorPage.h"

glm::mat4 transformation;

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
	this->projectionMatrix = glm::ortho(0.0f, this->screenWidth, 0.0f, this->screenHeight);

	transformation = glm::scale(transformation, glm::vec3(2.0f, 2.0f, 1.0f));

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
	this->gridCellShader.use();
	int m_colorUniform = this->gridCellShader.getUniformLocation("u_Color");
	int m_projectionUniform = this->gridCellShader.getUniformLocation("u_Projection");

	glm::mat4 m_transformed = transformation * this->projectionMatrix;

	glUniform4f(m_colorUniform, 1.0f, 0.0f, 1.0f, 1.0f);
	glUniformMatrix4fv(m_projectionUniform, 1, GL_FALSE, &m_transformed[0][0]);

	glBindVertexArray(this->cellVaoBuffer);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	// Render all of the cells
	//this->RenderCells();

	// Render the grid as the last item, this way the grid becomes an overlay
	//this->RenderGrid();
}

void SimulatorPage::InitGrid()
{

}

void SimulatorPage::HandleInput(GLFWwindow* a_window)
{
	if (glfwGetKey(a_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(this->window, true);
}

void SimulatorPage::MouseHover(GLFWwindow* a_window, double a_posX, double a_posY)
{

}

void SimulatorPage::MouseClick(GLFWwindow* a_window, int a_button, int a_action, int a_mods)
{

}