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
	this->gridLineShader.SetVertexShader("shaders/gridLineVertexShader.glsl");
	this->gridLineShader.SetFragmentShader("shaders/basicFragmentShader.glsl");
	this->gridLineShader.Compile();
	this->GetError(__LINE__);

	// Compile the gridCell shader, this shader is going to be used to render the grid cells
	this->gridCellShader.SetVertexShader("shaders/gridCellVertexShader.glsl");
	this->gridCellShader.SetFragmentShader("shaders/gridCellFragmentShader.glsl");
	this->gridCellShader.Compile();
	this->GetError(__LINE__);
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
	glGenBuffers(1, &this->cellOffsetBuffer);
	glGenBuffers(1, &this->cellColorBuffer);
	
	glBindVertexArray(this->cellVaoBuffer);
	
	// Fill the buffers with data
	glBindBuffer(GL_ARRAY_BUFFER, this->cellVboBuffer);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(glm::vec2), this->cellVertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->cellEboBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), this->cellIndices, GL_STATIC_DRAW);

	// Define the data layout for the GPU
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);

	// Define instance data
	glBindBuffer(GL_ARRAY_BUFFER, this->cellOffsetBuffer);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (GLvoid*)0); // Each element is a GL_FLOAT, they make a glm::vec2 with 2 elements, and the pointer
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glVertexAttribDivisor(2, 1); // One glm:vec2 at a time
	
	glBindBuffer(GL_ARRAY_BUFFER, this->cellColorBuffer);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (GLvoid*)0); // Each element is a GL_FLOAT, they make a glm::vec2 with 2 elements, and the pointer
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glVertexAttribDivisor(3, 1); // One glm:vec2 at a time

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
	this->imguiIO->WantCaptureKeyboard = true;

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
				ImGuiFileDialog::Instance()->OpenDialog("saveWorldFile", "Save world file", ".csv", "");
			if (ImGui::MenuItem("Exit to menu")) 
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

		if (ImGui::Button("Clear head and tails to conductors"))
			this->worldCells.ResetToConductors();

		if (ImGui::SliderFloat("Target speed", &targetSimulationSpeed, 0.01f, 256, "%.2f", 5.0f))
			this->worldCells.SetTargetSpeed(targetSimulationSpeed);
	}
	// Legacy API style not yet fixed by ImGui
	ImGui::End();

	if (brushWindowOpen)
	{
		if (ImGui::Begin("Brush", false, m_defaultWindowArgs))
		{
			ImGui::Columns(2, "", false);
			bool m_preState = ImGui::IsItemClicked(0);
			ImGui::Text("Type");
			ImGui::SetNextItemWidth(100);
			if (ImGui::IsItemClicked() != m_preState && !m_preState)
				this->pixeledView = !this->pixeledView;

			if (ImGui::ListBox("", &this->selectedCellDrawName, this->cellDrawStateNames, 4))
			{
				this->cellDrawState = (CellState)this->selectedCellDrawName;
			}
			ImGui::NextColumn();
			ImGui::Text("Brush thickness");
			ImGui::SetNextItemWidth(100);
			// Usage of space is required otherwise items will be added to previous list box (where the name is also "")
			// because it will create a ImGui ID from the name.
			if (ImGui::ListBox(" ", &this->brushRadiusSelectorPos, this->brushRadiusNames, 6))
			{
				// See proof below
				this->brushRadius = (this->brushRadiusSelectorPos * 2) + 1;
				/*
				1
					return ( 0 * 2) +1; = 1
				3
					return (1 *2) +1; = 3;
				5
					return (2 * 2) + 1; = 5;
				7
					return (3 * 2) + 1; = 7
				9
					return (4 * 2) + 1 = 9
				11
					return (5 * 2) + 1 = 11
				*/
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
			ImGui::Text("Head: ");
			ImGui::Text("Tail: ");
			ImGui::Text("Last update cycle time (ms): ");
			ImGui::Text("FPS:");
			ImGui::Text("Generation:");
			ImGui::NextColumn();
			if (this->worldCells.GetIsRunning())
				ImGui::Text("Running");
			else
				ImGui::Text("Paused");

			auto m_cellStats = this->worldCells.GetStatistics();
			ImGui::Text("%i", m_cellStats[2]); //Conductor count
			ImGui::Text("%i", m_cellStats[0]); // Head count
			ImGui::Text("%i", m_cellStats[1]); // Tail count
			ImGui::Text("%.4f", this->worldCells.lastUpdateDuration);
			ImGui::Text("%.4f", this->imguiIO->Framerate);
			ImGui::Text("%i", this->worldCells.GetDisplayGeneration());
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
				m_cell.second->InitRender(this->gridCellShader);

			auto m_topLeft = this->worldCells.GetTopLeftCoordinates();
			this->scrollOffsetX = -(m_topLeft.first - 1);
			this->scrollOffsetY = -(m_topLeft.second - 2);
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
#ifdef __keybinds__
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

	if (glfwGetKey(a_window, GLFW_KEY_4) == GLFW_PRESS)
	{
		if (!m_lastPressedKey4)
		{
			this->worldCells.Open("world.csv");
			for (auto m_cell : this->worldCells.cells)
				m_cell.second->InitRender(this->gridCellShader, this->cellVaoBuffer);
		}
		m_lastPressedKey4 = true;
	}
	else
		m_lastPressedKey4 = false;
#endif
}

void SimulatorPage::MouseHover(GLFWwindow* a_window, double a_posX, double a_posY)
{
	int m_cellSizeInPx = this->pixeledView ? 1 : this->cellSizeInPx;
	coordinatePart m_curCellXHovered = (((coordinatePart)this->gridLineSizeInPx + (coordinatePart)a_posX) / (coordinatePart)m_cellSizeInPx);
	coordinatePart m_curCellYHovered = (((coordinatePart)this->gridLineSizeInPx + (coordinatePart)a_posY) / (coordinatePart)m_cellSizeInPx);
	
	static coordinatePart m_lastCellX = 0;
	static coordinatePart m_lastCellY = 0;
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
			this->AddCellToWorld(this->curCellHoveredX, this->curCellHoveredY);
			// TODO. maybe calculate a border line at which we know that there are already items (from the original click event for or previous drag)
			if (this->brushRadius > 1)
			{
				int m_middle = this->brushRadius / 2;
				for (int m_xOffset = -m_middle; m_xOffset <= m_middle; m_xOffset++)
				{
					for (int m_yOffset = -m_middle; m_yOffset <= m_middle; m_yOffset++)
					{
						this->AddCellToWorld(this->curCellHoveredX + m_xOffset, (this->curCellHoveredY + m_yOffset));
					}
				}
			}
		}
	}
	
	// Scrolling
	if (this->rightMouseButtonIsDown || this->scrollWheelButtonIsDown)
	{
		coordinatePart m_diffX = m_curCellXHovered - m_lastCellX;
		coordinatePart m_diffY = m_curCellYHovered - m_lastCellY;
		this->scrollOffsetX += m_diffX;
		this->scrollOffsetY += m_diffY;
	}

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
		this->AddCellToWorld(this->curCellHoveredX, this->curCellHoveredY);
		if (this->brushRadius > 1)
		{
			int m_middle = this->brushRadius / 2;
			for (int m_xOffset = -m_middle; m_xOffset <= m_middle; m_xOffset++)
			{
				for (int m_yOffset = -m_middle; m_yOffset <= m_middle; m_yOffset++)
				{
					this->AddCellToWorld(this->curCellHoveredX + m_xOffset, (this->curCellHoveredY + m_yOffset));
				}
			}
		}
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
	if (!this->manuallyAddKeycodesToImgui)
		return;
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
		this->imguiIO->AddInputCharacter((char)a_key);
	}
}

void SimulatorPage::RenderCells()
{
	// Get all of the cells that are located within the viewport
	coordinatePart m_viewportOriginX = -1 - this->scrollOffsetX;
	coordinatePart m_viewportOriginY = -1 - this->scrollOffsetY;

	int m_cellSizeInPx = this->pixeledView ? 1 : this->cellSizeInPx;

	long m_viewportWidth = (this->screenWidth / m_cellSizeInPx) + 2;
	long m_viewportHeight = (this->screenHeight / m_cellSizeInPx) + 2;

	static std::vector<Cell*> m_cellsInViewport;
	m_cellsInViewport.clear();
	this->worldCells.InViewport(&m_cellsInViewport, m_viewportOriginX, m_viewportOriginY, m_viewportWidth, m_viewportHeight);

	// Set the projection matrix
	this->gridCellShader.Use();
	this->gridCellShader.SetMatrixValue("u_ProjectionMatrix", &this->projectionMatrix[0][0]);

	// Set the scaling matrix for the cell
	glm::mat4 m_scaleMatrix = glm::scale(glm::vec3(m_cellSizeInPx, m_cellSizeInPx, 0.0f));
	this->gridCellShader.SetMatrixValue("u_ModelMatrix", &m_scaleMatrix[0][0]);
	
	// Render all of the world cells
	int m_pendingCellRenders = 0;
	for (auto m_worldCell : m_cellsInViewport)
	{
		if(m_worldCell != nullptr)
		{
			// Set the VAO
			glBindVertexArray(this->cellVaoBuffer);
			
			// Get the latest color and offsets at their place in the array
			m_worldCell->Render(m_cellSizeInPx, this->scrollOffsetX, this->scrollOffsetY, &this->cellOffsets[m_pendingCellRenders], &this->cellColors[m_pendingCellRenders]);
			m_pendingCellRenders++;

			if (m_pendingCellRenders == InstanceBufferSize)
			{
				// If we filled all the buffers, copy them to the GPU, render them and start over again
				this->UpdateAndRenderPendingCells(m_pendingCellRenders);
				m_pendingCellRenders = 0;
			}
		}
	}

	// If there are any cells that where left in the buffer, send them to GPU and render them
	if (m_pendingCellRenders > 0)
	{
		this->UpdateAndRenderPendingCells(m_pendingCellRenders);
		m_pendingCellRenders = 0;
	}
}

void SimulatorPage::UpdateAndRenderPendingCells(int a_pendingCellRenders)
{
	glBindVertexArray(this->cellVaoBuffer);

	glBindBuffer(GL_ARRAY_BUFFER, this->cellOffsetBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * InstanceBufferSize, &this->cellOffsets[0], GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, this->cellColorBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * InstanceBufferSize, &this->cellColors[0], GL_DYNAMIC_DRAW);
	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0, a_pendingCellRenders);

	glBindVertexArray(0);
}

void SimulatorPage::RenderGrid()
{
	if (this->pixeledView)
		return;

	this->gridLineShader.Use();
	
	// Get the uniforms
	int m_projectionMatrixUniform = this->gridLineShader.GetUniformLocation("u_ProjectionMatrix");
	int m_drawHorizontalUniform = this->gridLineShader.GetUniformLocation("u_DrawHorizontal");
	int m_cellSizeInPxUniform = this->gridLineShader.GetUniformLocation("u_CellSizeInPx");
	int m_colorUniform = this->gridLineShader.GetUniformLocation("u_Color");

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

void SimulatorPage::AddCellToWorld(coordinatePart a_x, coordinatePart a_y)
{
	CellState m_cellState = this->cellDrawState; 
	if (m_cellState == Background)
		this->RemoveCellFromWorld(a_x, a_y);
	else
	{
		bool m_doInit = false;
		if (this->worldCells.TryInsertCellAt(a_x, a_y, m_cellState))
			m_doInit = true;

		Shader* m_shader = &this->gridCellShader;
		this->worldCells.TryUpdateCell(a_x, a_y, 
			[m_cellState, m_doInit, m_shader](Cell* a_foundCell) -> bool 
			{ 
				if (m_doInit)
					a_foundCell->InitRender(*m_shader);

				a_foundCell->cellState = m_cellState;
				return true;
			}
		);
	}
}

void SimulatorPage::RemoveCellFromWorld(coordinatePart a_x, coordinatePart a_y)
{
	this->worldCells.TryDeleteCell(a_x, a_y);
}