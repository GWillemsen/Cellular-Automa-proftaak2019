#include <iostream>
#include <glm/glm.hpp>
#include "imgui.h"
#ifndef __CONFIG__
#define __CONFIG__

class Config
{
public:
	glm::vec4 backgroundColor = glm::vec4(0.017241379310344827f, 0.017241379310344827f, 0.017241379310344827f, 1.0f); // Color of the background
	glm::vec3 conductorColor = glm::vec3(1.0f, 1.0f, 0.0f); // Color of a conductor, default yellow
	glm::vec3 headColor = glm::vec3(1.0f, 0.0f, 0.0f); // Color of a head, default red
	glm::vec3 tailColor = glm::vec3(0.0f, 0.0f, 1.0f); // Color of a tail, default blue
	std::string templateFolder = "/templates";
	
private:
	const ImVec4 activeWindowTitleBgColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
	const ImVec4 windowTitleBgColor	=		ImVec4(1.0f, 0.0f, 0.0f, .8f);
	const ImVec4 buttonBgColor =			ImVec4(.4f, .4f, .4f, 0.5f);
	const ImVec4 hoverButtonBgColor =		ImVec4(.4f, .4f, .4f, 0.9f);
	const ImVec4 activeButtonBgColor =		ImVec4(.5f, .5f, .5f, 0.9f);
	const ImVec4 menuBarBgColor =			ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
	const ImVec4 frameBgColor =				ImVec4(0.4f, 0.4f, 0.4f, 0.5f);
	const ImVec4 hoverFrameBgColor =		ImVec4(0.4f, 0.4f, 0.4f, 0.9f);
	const ImVec4 activeFrameBgColor =		ImVec4(0.5f, 0.5f, 0.5f, 0.9f);

public:
	static Config* instance ;
	void SetImGuiStyle(ImGuiStyle* a_imguiStyle)
	{
		a_imguiStyle->Colors[ImGuiCol_TitleBg] = this->windowTitleBgColor;
		a_imguiStyle->Colors[ImGuiCol_TitleBgActive] = this->activeWindowTitleBgColor;
		a_imguiStyle->Colors[ImGuiCol_Button] = this->buttonBgColor;
		a_imguiStyle->Colors[ImGuiCol_ButtonActive] = this->activeButtonBgColor;
		a_imguiStyle->Colors[ImGuiCol_ButtonHovered] = this->hoverButtonBgColor;
		a_imguiStyle->Colors[ImGuiCol_MenuBarBg] = this->menuBarBgColor;
		a_imguiStyle->Colors[ImGuiCol_FrameBg] = this->frameBgColor;
		a_imguiStyle->Colors[ImGuiCol_FrameBgActive] = this->activeFrameBgColor;
		a_imguiStyle->Colors[ImGuiCol_FrameBgHovered] = this->hoverFrameBgColor;
		a_imguiStyle->Colors[ImGuiCol_Header] = this->buttonBgColor;
		a_imguiStyle->Colors[ImGuiCol_HeaderActive] = this->activeButtonBgColor;
		a_imguiStyle->Colors[ImGuiCol_HeaderHovered] = this->hoverButtonBgColor;
	}

private:
	Config() {}; // Constructor
};


#endif // !__CONFIG__
