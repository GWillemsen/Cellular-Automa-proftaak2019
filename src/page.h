#include <GLAD/glad.h>
#include <GLFW/glfw3.h>
#include <typeinfo>
#include "config.h"

#ifndef __PAGE__
#define __PAGE__

class Page
{
public:
	GLFWwindow* window;
	int screenWidth;
	int screenHeight;
	std::string pageName;
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
			this->lastLineNumber = a_line;
		}
	}
};

#endif // !__PAGE__
