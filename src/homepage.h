/*
MIT License

Copyright (c) 2020 Guylian Gilsing & Giel Willemsen

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

// System libraries
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


	virtual void MouseHover(GLFWwindow* a_window, double a_posX, double a_posY) override {}
	virtual void MouseClick(GLFWwindow* a_window, int a_button, int a_action, int a_mods) override {}
	virtual void MouseScroll(GLFWwindow* a_window, double a_xOffset, double a_yOffset) override {}
};

#endif // !__HOMEPAGE__
