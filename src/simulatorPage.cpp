// Standard system libraries
#include <string>
#include <array>
#include "simulatorPage.h"
#include "homepage.h"

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

	while (!this->closeThisPage && !glfwWindowShouldClose(this->window))
	{
		glfwPollEvents();

		// Clear the screen
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Logic
		this->HandleInput(this->window);
		this->RenderOpenGL();
		this->RenderImGui();

		glfwSwapBuffers(this->window);
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	return this->nextPage;
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
  
	this->imguiIO = &ImGui::GetIO(); (void)this->imguiIO;
	this->imguiIO.WantCaptureKeyboard = true;

	// Enable keyboard input
	this->imguiIO->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

	ImGui::StyleColorsDark();

	// Setup renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(this->window, true);
	ImGui_ImplOpenGL3_Init(this->glsl_version);
	ImGui::LoadIniSettingsFromDisk("imgui.ini");
	Config::instance->SetImGuiStyle(&ImGui::GetStyle());
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

void SimulatorPage::RenderImGui()
{
	// Renders the GUI with Dear ImGUI
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::CaptureKeyboardFromApp(true);
		
	const ImGuiWindowFlags m_defaultWindowArgs = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize;
  
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Open"))
				ImGuiFileDialog::Instance()->OpenDialog("chooseWorldFile", "Choose world file", ".csv", "");
			if (ImGui::MenuItem("Save"))
				ImGuiFileDialog::Instance()->OpenDialog("saveWorldFile", "Save world file", ".csv", "world.csv");
			if (ImGui::MenuItem("Back to menu")) 
			{
				this->nextPage = new HomePage(this->window);
				this->closeThisPage = true;
			}
			if (ImGui::MenuItem("Exit"))
			{
				this->closeThisPage = true;
				glfwSetWindowShouldClose(this->window, 1);
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("View"))
		{
			ImGui::MenuItem("Debug window", nullptr, &debugWindowOpen);
			ImGui::MenuItem("Brushes window", nullptr, &brushWindowOpen);

			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
	
	if (ImGui::Begin("World options", false, m_defaultWindowArgs))
	{
		if (this->worldCells.GetIsRunning())
		{
			ImGui::Text("Start");
			if (ImGui::Button("Stop"))
				this->worldCells.PauzeSimulation();
		}
		else
		{
			if (ImGui::Button("Start"))
				this->worldCells.StartSimulation();
			ImGui::Text("Stop");
		}
		if (ImGui::Button("Reset"))
			this->worldCells.ResetSimulation();

		if (ImGui::SliderFloat("Target speed", &targetSimulationSpeed, 0.01, 256, "%.2f", 5.0f))
			this->worldCells.SetTargetSpeed(targetSimulationSpeed);
	}
	// Legacy API style not yet fixed by ImGui
	ImGui::End();

	if (brushWindowOpen)
	{
		if (ImGui::Begin("Brush", false, m_defaultWindowArgs))
		{
			ImGui::SetNextItemWidth(100);
			if (ImGui::ListBox("", &this->selectedCellDrawName, this->cellDrawStateNames, 4))
			{
				this->cellDrawState = (CellState)this->selectedCellDrawName;
			}
		}
		// Legacy API style not yet fixed by ImGui
		ImGui::End();
	}

	if (debugWindowOpen)
	{
		if (ImGui::Begin("Debug", false, m_defaultWindowArgs))
		{
			ImGui::Columns(2);
			ImGui::SetColumnWidth(0, 200);
			ImGui::SetColumnWidth(1, 80);

			ImGui::Text("Status: ");
			ImGui::Text("Conductor: ");
			ImGui::Text("Tail: ");
			ImGui::Text("Head: ");
			ImGui::Text("Last update cycle time (ms): ");
			ImGui::Text("FPS:");
			ImGui::NextColumn();
			if (this->worldCells.GetIsRunning())
				ImGui::Text("Running");
			else
				ImGui::Text("Paused");

			auto m_cellStats = this->worldCells.GetStatistics();
			ImGui::Text("%i", m_cellStats[2]); //Conductor count
			ImGui::Text("%i", m_cellStats[1]); // Tail count
			ImGui::Text("%i", m_cellStats[0]); // Head count
			ImGui::Text("%.4f", this->worldCells.lastUpdateDuration);
			ImGui::Text("%.4f", this->imguiIO->Framerate);
		}
		// Legacy API style not yet fixed by ImGui
		ImGui::End();
	}
	this->isInImguiWindow = ImGui::IsAnyWindowHovered();

	// Display the choose file browser dialog
	if (ImGuiFileDialog::Instance()->FileDialog("chooseWorldFile"))
	{
		// IF the action is OK we will get the chosen file from the file dialog
		if (ImGuiFileDialog::Instance()->IsOk == true)
		{
			// Get the path and name of the file and open it with the world.h API
			std::string m_filePathName = ImGuiFileDialog::Instance()->GetFilepathName();

			// Open the file
			if (m_filePathName != "")
				this->worldCells.Open(m_filePathName);

			// Initialize the rendering of all the newly added cells
			for (auto m_cell : this->worldCells.cells)
				m_cell.second->InitRender(this->gridCellShader, this->cellVaoBuffer);
		}

		// close
		ImGuiFileDialog::Instance()->CloseDialog("chooseWorldFile");
	}

	// Display the choose file browser dialog
	if (ImGuiFileDialog::Instance()->FileDialog("saveWorldFile"))
	{
		// IF the action is OK we will get the chosen file from the file dialog
		if (ImGuiFileDialog::Instance()->IsOk == true)
		{
			// Get the path and name of the file and open it with the world.h API
			std::string m_filePathName = ImGuiFileDialog::Instance()->GetFilepathName();

			// Save the file
			if (m_filePathName != "")
			{
				this->worldCells.filePath = m_filePathName;
				this->worldCells.Save();
			}
		}

		// close
		ImGuiFileDialog::Instance()->CloseDialog("saveWorldFile");
	}

	ImGui::Render();

	ImGui::EndFrame();	

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void SimulatorPage::HandleInput(GLFWwindow* a_window)
{
	if (glfwGetKey(a_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(this->window, true);

	if (glfwGetKey(a_window, GLFW_KEY_S) == GLFW_PRESS)
		this->worldCells.StartSimulation();

	if (glfwGetKey(a_window, GLFW_KEY_P) == GLFW_PRESS)
		this->worldCells.PauzeSimulation();

	static bool m_lastPressedKey1 = false;
	static bool m_lastPressedKey2 = false;
	static bool m_lastPressedKey3 = false;
	static bool m_lastPressedKey4 = false;
	if (glfwGetKey(a_window, GLFW_KEY_2) == GLFW_PRESS)
	{
		if (!m_lastPressedKey2)
		{
			float m_speed = this->worldCells.GetTargetSpeed();
			m_speed += 2.0f;
			this->worldCells.SetTargetSpeed(m_speed);
			m_lastPressedKey2 = true;
		}
	}
	else 
	{
		m_lastPressedKey2 = false;
	}

	if (glfwGetKey(a_window, GLFW_KEY_1) == GLFW_PRESS)
	{
		if (!m_lastPressedKey1)
		{
			float m_speed = this->worldCells.GetTargetSpeed();
			if (m_speed - 0.1f < 0)
				m_speed = 0.01f;
			else if (m_speed - 2.0f >= 1.0f)
				m_speed -= 2.0f;
			else
				m_speed -= 0.1f;
			this->worldCells.SetTargetSpeed(m_speed);
			m_lastPressedKey1 = true;
		}
	}
	else
	{
		m_lastPressedKey1 = false;
	}


	if (glfwGetKey(a_window, GLFW_KEY_3) == GLFW_PRESS)
	{
		if (!m_lastPressedKey3)
		{
			this->worldCells.Save();
		}
		m_lastPressedKey3 = true;
	}
	else
		m_lastPressedKey3 = false;

	//if (glfwGetKey(a_window, GLFW_KEY_4) == GLFW_PRESS)
	//{
	//	if (!m_lastPressedKey4)
	//	{
	//		this->worldCells.Open("world.csv");
	//		for (auto m_cell : this->worldCells.cells)
	//			m_cell.second->InitRender(this->gridCellShader, this->cellVaoBuffer);
	//	}
	//	m_lastPressedKey4 = true;
	//}
	//else
	//	m_lastPressedKey4 = false;
}

void SimulatorPage::MouseHover(GLFWwindow* a_window, double a_posX, double a_posY)
{
    long m_curCellXHovered = (((long)(this->gridLineSizeInPx + a_posX) / this->cellSizeInPx));
	long m_curCellYHovered = (((long)(this->gridLineSizeInPx + a_posY) / this->cellSizeInPx));
	
	static long long m_lastCellX = 0;
	static long long m_lastCellY = 0;
	static double m_lastMouseX = 0;
	static double m_lastMouseY = 0;

	// Calculate in which cell the mouse cursor is located
	if (this->curCellHoveredX != m_curCellXHovered - this->scrollOffsetX)
	{
		this->curCellHoveredX = m_curCellXHovered - this->scrollOffsetX;
	}

	if (this->curCellHoveredY != m_curCellYHovered - this->scrollOffsetY)
	{
		this->curCellHoveredY = m_curCellYHovered - this->scrollOffsetY;
	}
  
	// Add / remove cells while the mouse is being dragged around
	if (this->leftMouseButtonIsDown)
	{
		if (m_lastCellX != m_curCellXHovered || m_lastCellY != m_curCellYHovered)
		{
			this->AddCellToWorld(true);
		}
	}
	else if (this->rightMouseButtonIsDown)
	{
		if (m_lastCellX != m_curCellXHovered || m_lastCellY != m_curCellYHovered)
		{
			this->RemoveCellFromWorld();
		}
	}

	// Scrolling	
	if (this->scrollWheelButtonIsDown)
	{
		double m_scrollDifferenceX = a_posX - m_lastMouseX;
		double m_scrollDifferenceY = a_posY - m_lastMouseY;

		if (this->curCellHoveredX != m_lastCellX || this->curCellHoveredY != m_lastCellY)
		{
			if (m_scrollDifferenceX > 0.001)
				this->scrollOffsetX += 1;
			else if (m_scrollDifferenceX < -0.001)
				this->scrollOffsetX -= 1;

			if (m_scrollDifferenceY > 0.001)
				this->scrollOffsetY += 1;
			else if (m_scrollDifferenceY < -0.001)
				this->scrollOffsetY -= 1;
		}
	}

	// These should be on the bottom as we use them to check if we changed something from frame to frame.
	// (and therefor should always update at the end of each frame).
	m_lastMouseX = a_posX;
	m_lastMouseY = a_posY;
	m_lastCellX = m_curCellXHovered;
	m_lastCellY = m_curCellYHovered;
}

void SimulatorPage::MouseClick(GLFWwindow* a_window, int a_button, int a_action, int a_mods)
{
	if (this->isInImguiWindow)
		return;

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

	// Scroll wheel button press
	if (a_button == 2 && a_action == 1)
	{
		this->scrollWheelButtonIsDown = true;
	}
	// Scroll wheel button release
	else if (a_button == 2 && a_action == 0)
	{
		this->scrollWheelButtonIsDown = false;
	}
}

void SimulatorPage::MouseScroll(GLFWwindow* a_window, double a_xOffset, double a_yOffset)
{
	if (this->isInImguiWindow)
		return;
	
	// Zoom in
	if (a_yOffset > 0 && this->cellSizeInPx < 128)
	{
		this->cellSizeInPx += 2;
	}
	// Zoom out
	else if (a_yOffset < 0 && this->cellSizeInPx > 16)
	{
		this->cellSizeInPx -= 2;
	}
}

void SimulatorPage::KeyPress(GLFWwindow* a_window, int a_key, int a_scancode, int a_action, int a_mods)
{
	if (a_key <= CHAR_MAX)
	{
		if (a_mods == 0) // No modifier
		{
			// normal keys
			if (a_key >= 65 && a_key <= 90)
				a_key += 32;
		}
		else
		{
			// key 1 tot 5 above qwerty
			if (a_key >= 49 && a_key <= 53 && a_key != 50 /*see below*/)
				a_key -= 16;
			// 2 -> 50
			else if (a_key == 50)
				a_key = 64;
			// 6 -> 38
			else if (a_key == 54)
				a_key = 94;
			//7 -> 55
			else if (a_key == 55)
				a_key = 38;
			// 8 -> 56
			else if (a_key == 56)
				a_key = 42;
			// 9 -> 57
			else if (a_key == 57)
				a_key = 40;
			//0 -> 48
			else if (a_key == 48)
				a_key = 41;
			// _ -> 45
			else if (a_key == 45)
				a_key = 95;
			// + -> 61
			else if (a_key == 61)
				a_key = 43;
		}
	
		//const std::wstring wide_string = std::wstring((wchar_t)a_key);
		//convert
		this->imguiIO.AddInputCharacter((char)a_key);
	}
}

void SimulatorPage::RenderCells()
{
	// Get all of the cells that are located within the viewport
	int m_viewportOriginX = -1 - this->scrollOffsetX;
	int m_viewportOriginY = -1 - this->scrollOffsetY;

	int m_viewportWidth = (this->screenWidth / this->cellSizeInPx) + 2;
	int m_viewportHeight = (this->screenHeight / this->cellSizeInPx) + 2;

	std::vector<Cell*> m_cellsInViewport;
	this->worldCells.InViewport(&m_cellsInViewport, m_viewportOriginX, m_viewportOriginY, m_viewportWidth, m_viewportHeight);

	// Set the projection matrix
	this->gridCellShader.use();
	int m_projectionMatrixUniform = this->gridCellShader.getUniformLocation("u_ProjectionMatrix");

	glUniformMatrix4fv(m_projectionMatrixUniform, 1, GL_FALSE, &this->projectionMatrix[0][0]);

	// Render all of the worldcells
	for (auto m_worldCell : m_cellsInViewport)
	{
		if(m_worldCell != nullptr)
			m_worldCell->Render(this->cellSizeInPx, this->scrollOffsetX, this->scrollOffsetY);
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
	CellState m_cellState = this->cellDrawState; // Gets manipulated based on logic
	// Change cell state
	if (m_cellState == Background)
		this->RemoveCellFromWorld();
	else
	{
		bool m_doInit = false;
		if (this->worldCells.TryInsertCellAt(this->curCellHoveredX, this->curCellHoveredY, m_cellState))
			m_doInit = true;
		GLint m_vaoBuffer = this->cellVaoBuffer;
		Shader* m_shader = &this->gridCellShader;
		this->worldCells.TryUpdateCell(this->curCellHoveredX, this->curCellHoveredY, [m_cellState, m_doInit, m_vaoBuffer, m_shader](Cell* a_foundCell) -> bool 
		{ 
			if (m_doInit)
				a_foundCell->InitRender(*m_shader, m_vaoBuffer);

			a_foundCell->cellState = m_cellState;
			return true;
		});
	}

}

void SimulatorPage::RemoveCellFromWorld()
{
	this->worldCells.TryDeleteCell(this->curCellHoveredX, this->curCellHoveredY);
}