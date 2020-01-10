#include <iostream>
#include <glm/glm.hpp>
#include "imgui.h"
#ifndef __CONFIG__
#define __CONFIG__

class Config
{
public:
	glm::vec4 backgroundColor = glm::vec4(0.017241379310344827f, 0.017241379310344827f, 0.017241379310344827f, 1.0f); // Color of the background
	glm::vec4 guideColor = glm::vec4(0.00392156862745098f, 0.005128205128205128f, 0.05555555555555555f, 1.0f); // Color of a conductor
	glm::vec4 headColor = glm::vec4(0.16666666666666666f, 0.012195121951219513f, 0.004524886877828055f, 1.0f); // Color of a head
	glm::vec4 tailColor = glm::vec4(0.004273504273504274f, 0.03125f, 0.02564102564102564f, 1.0f); // Color of a tail
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
