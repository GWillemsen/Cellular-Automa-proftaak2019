// GLAD, GLM and GLFW
#include <glad\glad.h>
#include <GLFW\glfw3.h>

// Dear ImGui
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

// Class header files
#include "page.h"
#include "simulatorPage.h"

#include "homepage.h"

HomePage::HomePage(GLFWwindow* a_window) : Page(a_window, "Homepage")
{
	this->nextPage = nullptr ;
}


Page* HomePage::Run()
{
	this->InitImGui();

	while (!this->closeThisPage && !glfwWindowShouldClose(this->window))
	{
		glfwPollEvents();

		// Clear the screen
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Logic
		this->RenderImGui();

		glfwSwapBuffers(this->window);
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	return this->nextPage;
}

void HomePage::InitImGui() 
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
	ImGui::LoadIniSettingsFromDisk("imgui_homepage.ini");
}

void HomePage::RenderImGui() 
{
	const float m_itemSpacing = ImGui::GetStyle().ItemSpacing.x;
	const float m_buttonWidth = 250.0f;
	const char* m_wireWorldName = "Wire World";
	const char* m_createdByText = "Created by Guylian Gilsing & Giel Willemsen";
	const char* m_copyRight = "(C) 2020";
	static float m_measureHeaderSize = 0;
	
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	if (ImGui::Begin("Window", false, ImGuiWindowFlags_NoTitleBar))
	{
		ImGui::SetWindowSize("Window", ImVec2(this->screenWidth, this->screenHeight));
		ImGui::SetWindowPos("Window", ImVec2(0, 0));
		ImGui::Columns(3);
		
		float m_middleColumnWidth = (ImGui::GetWindowWidth() / 6) * 4;
		float m_windowWidth = ImGui::GetWindowWidth();
		float m_smallerSideColumnsValue = 0;
		if(m_middleColumnWidth < m_measureHeaderSize)
		{
			m_smallerSideColumnsValue = ( m_measureHeaderSize - m_middleColumnWidth) / 2;
			m_middleColumnWidth = m_measureHeaderSize + 5;
		}
		float m_smallColWidth = (ImGui::GetWindowWidth() / 6) - m_smallerSideColumnsValue;
		ImGui::SetColumnWidth(0, m_smallColWidth);
		ImGui::SetColumnWidth(1, m_middleColumnWidth);
		ImGui::SetColumnWidth(2, m_smallColWidth);
		
		// goto the 2nd column
		ImGui::NextColumn();

		// Pad something from the top
		ImGui::NewLine();
		ImGui::NewLine();
		// in second column, each in one row
		ImGui::SetWindowFontScale(9);
		float m_textWidth = ImGui::CalcTextSize(m_wireWorldName).x + 5;
		m_measureHeaderSize = m_textWidth;
		ImGui::SameLine((m_middleColumnWidth - m_textWidth) / 2);
		ImGui::Text(m_wireWorldName);

		// next row
		// Pad something extra
		ImGui::NewLine();
		ImGui::SetWindowFontScale(2);
		m_textWidth = ImGui::CalcTextSize(m_createdByText).x;
		ImGui::SameLine((m_middleColumnWidth - m_textWidth) / 2);
		ImGui::Text(m_createdByText);
			
		ImGui::NewLine();
		m_textWidth = ImGui::CalcTextSize(m_copyRight).x;
		ImGui::SameLine((m_middleColumnWidth - m_textWidth) / 2);
		ImGui::Text(m_copyRight);
		
		//Next row + some padding
		ImGui::NewLine();
		ImGui::NewLine();
		ImGui::SetWindowFontScale(2);
		if (ImGui::Button("Open editor", ImVec2(m_buttonWidth,100)))
		{
			this->nextPage = new SimulatorPage(this->window);
			this->closeThisPage = true;
		}
		
		float m_padding = m_buttonWidth + m_itemSpacing;
		ImGui::SameLine(m_middleColumnWidth - m_padding);
		
		if (ImGui::Button("Exit", ImVec2(m_buttonWidth,100)))
		{
			this->closeThisPage = true;
		}

		ImGui::SetWindowFontScale(1);
		
	}
	ImGui::End();
	ImGui::Render();
	ImGui::EndFrame();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void HomePage::MouseHover(GLFWwindow* a_window, double a_posX, double a_posY) 
{

}

void HomePage::MouseClick(GLFWwindow* a_window, int a_button, int a_action, int a_mods) 
{

}

void HomePage::MouseScroll(GLFWwindow* a_window, double a_xOffset, double a_yOffset) 
{

}

