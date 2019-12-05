#include <GLAD/glad.h>
#include <GLFW/glfw3.h>

#ifndef __PAGE__
#define __PAGE__

class Page
{
public:
	GLFWwindow* window;
	int screenWidth;
	int screenHeight;

public:
	Page(GLFWwindow* a_window) 
	{
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
};

#endif // !__PAGE__
