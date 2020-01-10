// System libaries
#include <iostream>
#include <thread>
#include <map>

// GLAD, GLM and GLFW
#include <glad\glad.h>
#include <GLFW\glfw3.h>

// Class header files
#include "page.h"

#ifndef __HOMEPAGE__
#define __HOMEPAGE__

class HomePage : public Page
{
private:
	ImGuiIO imguiIO;
public:
	HomePage(GLFWwindow* a_window);
	virtual Page* Run() override;

private:
	void InitImGui();
	void RenderImGui();


	virtual void MouseHover(GLFWwindow* a_window, double a_posX, double a_posY) override;
	virtual void MouseClick(GLFWwindow* a_window, int a_button, int a_action, int a_mods) override;
	virtual void MouseScroll(GLFWwindow* a_window, double a_xOffset, double a_yOffset) override;
};

#endif // !__HOMEPAGE__
