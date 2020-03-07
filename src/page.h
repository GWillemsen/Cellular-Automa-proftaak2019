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

#include <GLAD/glad.h>
#include <GLFW/glfw3.h>
#include <typeinfo>
#include "config.h"

#ifndef __PAGE__
#define __PAGE__

class Page
{
public:
	const char* glsl_version = "#version 330 core";
	GLFWwindow* window;
	int screenWidth;
	int screenHeight;
	std::string pageName;
protected:
	Page* nextPage = nullptr;
	bool closeThisPage = false;
private:
	unsigned int lastLineNumber = 0;
public:
	Page(GLFWwindow* a_window, std::string a_pageName) 
	{
		this->pageName = a_pageName;
		this->window = a_window;
		this->screenHeight = 0;
		this->screenWidth = 0;
		glfwGetWindowSize(a_window, &this->screenWidth, &this->screenHeight);
	}
	virtual Page* Run() = 0;
	virtual void UpdateScreenSize(int a_newWidth, int a_newHeight) {
		this->screenHeight = a_newHeight;
		this->screenWidth = a_newWidth;
	}

	virtual void GetError(int a_line)
	{
		GLenum m_error = glGetError();
		if (m_error != GL_NO_ERROR)
		{
			std::cout << "A OpenGL call with error code " << m_error << " took place somewhere between line: " << this->lastLineNumber << " and " << a_line;
			std::cout << " most likely at page: " << this->pageName << std::endl;
		}
		this->lastLineNumber = a_line;
	}
	virtual void MouseHover(GLFWwindow* a_window, double a_posX, double a_posY) {};
	virtual void MouseClick(GLFWwindow* a_window, int a_button, int a_action, int a_mods) {};
	virtual void MouseScroll(GLFWwindow* a_window, double a_xOffset, double a_yOffset) {};
	virtual void KeyPress(GLFWwindow* a_window, int a_key, int a_scancode, int a_action, int a_mods) {};
	virtual ~Page() = default;
};

#endif // !__PAGE__
