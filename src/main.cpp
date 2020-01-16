// Include libs.
#include <thread>
#include <iostream>
#include <fstream>
#include <direct.h>

#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <math.h>
#include <chrono>
#include <glm/ext.hpp> //glm::value ptr
#include <glm/glm.hpp> //glm core
#include <glm/vec2.hpp> //glm vec2
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/ext/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale
#include <glm/ext/matrix_clip_space.hpp> // glm::perspective
#include <glm/gtx/string_cast.hpp>

#include "imgui.h"

#include "imgui_impl_glfw.h"

#include "imgui_impl_opengl3.h"

#include "shader.h"
#include "config.h"
#include "homepage.h"
#include "simulatorPage.h"
#include "resources.h"

// References.
void Framebuffer_size_callback(GLFWwindow* a_window, int a_width, int a_height);
void GetError(int a_line);
GLFWwindow* CreateWindow();
GLFWmonitor* GetSecondMonitor();
void InitGLFW();
bool TryInitGLAD();
unsigned int CreateVBO(float* a_indices, int a_verticeCount);
unsigned int CreateVAO();
unsigned int CreateEBO(unsigned int* indices, int a_indiceCount);

void MouseHover(GLFWwindow* a_window, double a_posX, double a_posY);
void mouseClick(GLFWwindow* a_window, int a_button, int a_action, int a_mods);
void MouseScroll(GLFWwindow* a_window, double a_xOffset, double a_yOffset);
void KeyPress(GLFWwindow* a_window, int a_key, int a_scancode, int a_action, int a_mods);

void CreateResources();

Page* nextPage = nullptr;
int screenWidth = 1920;
int screenHeight = 1080;

int main()
{
	CreateResources();

	InitGLFW();

	// ---
	// Create Window.
	// ---

	GLFWwindow* window = CreateWindow();
	if (window == nullptr)
		return -1;

	glfwMakeContextCurrent(window);

	if (!TryInitGLAD())
		return -1;

	// Set the view port
	glViewport(0, 0, screenWidth, screenHeight);

	// Callbacks.

	// Binds the 'framebuffer_size_callback' method to the window resize event.
	glfwSetFramebufferSizeCallback(window, Framebuffer_size_callback);

	// Set callbacks
	glfwSetMouseButtonCallback(window, mouseClick); // Catches the mouse click event
	glfwSetCursorPosCallback(window, MouseHover); // Catches the mouse move event
	glfwSetScrollCallback(window, MouseScroll);
	glfwSetKeyCallback(window, KeyPress);

	nextPage = new HomePage(window);

	while (nextPage)
	{
		Page* m_newNextPage = nextPage->Run();
		delete nextPage;
		nextPage = m_newNextPage;
	}
		
	// Cleanup
	glfwTerminate();
	return 0;
}

// Handle window resize.
void Framebuffer_size_callback(GLFWwindow* a_window, int a_width, int a_height)
{
	nextPage->UpdateScreenSize(a_width, a_height);

	// Update the view port
	glViewport(0, 0, a_width, a_height);
}

void GetError(int a_line)
{
	GLenum m_error = glGetError();
	if (m_error != GL_NO_ERROR)
		std::cout << m_error << " At line: " << a_line << std::endl;
}

GLFWwindow* CreateWindow()
{
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);

	// Set the update rate
	glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
	// Don't use any borders
	glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

	//GLFWwindow* window = glfwCreateWindow(1000, 1000, "LearnOpenGL", NULL, NULL);
	GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "LearnOpenGL", monitor, NULL);

	// Get the resulting screen width & height
	glfwGetWindowSize(window, &screenWidth, &screenHeight);

	// Make sure that the window is created.
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window." << std::endl;
		glfwTerminate();

		std::cin.get();
		return nullptr;
	}
	return window;
}

GLFWmonitor* GetSecondMonitor()
{
	int m_count = 0;
	auto m_monitors = glfwGetMonitors(&m_count);
	if (m_count > 1)
		return *(m_monitors + 1);
	else
		return nullptr;
}

void InitGLFW()
{
	// ---
	// Initialize GLFW.
	// ---
	glfwInit();


	// Tell GLFW that we want to use OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_MAXIMIZED, GL_TRUE);

	// Tell GLFW that we want to use the OpenGL's core profile.
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Do this for mac compatibility.
	// glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
}

bool TryInitGLAD()
{
	// ---
	// Initialize GLAD.
	// ---

	// Make sure that glad has been initialized successfully.
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD." << std::endl;

		std::cin.get();
		return false;
	}
	return true;
}

unsigned int CreateEBO(unsigned int* a_indices, int a_indiceCount)
{
	unsigned int eboBuffer;
	glGenBuffers(1, &eboBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, a_indiceCount * sizeof(unsigned int), a_indices, GL_STATIC_DRAW);
	return eboBuffer;
}

unsigned int CreateVBO(float* a_vertices, int a_verticeCount)
{
	unsigned int vboBuffer;
	// Create a VBO and fill it with data.
	glGenBuffers(1, &vboBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vboBuffer);
	glBufferData(GL_ARRAY_BUFFER, a_verticeCount * sizeof(glm::vec4), a_vertices, GL_STREAM_DRAW);
	return vboBuffer;
}

unsigned int CreateVAO()
{
	unsigned int renderVao;
	// Create a VAO.
	glGenVertexArrays(1, &renderVao);
	glBindVertexArray(renderVao);
	return renderVao;
}

void MouseHover(GLFWwindow* a_window, double a_posX, double a_posY)
{
	if (nextPage != nullptr)
		nextPage->MouseHover(a_window, a_posX, a_posY);
}

void mouseClick(GLFWwindow* a_window, int a_button, int a_action, int a_mods)
{
	if (nextPage != nullptr)
		nextPage->MouseClick(a_window, a_button, a_action, a_mods);
}

void MouseScroll(GLFWwindow* a_window, double a_xOffset, double a_yOffset)
{
	if (nextPage != nullptr)
		nextPage->MouseScroll(a_window, a_xOffset, a_yOffset);
}

void KeyPress(GLFWwindow* a_window, int a_key, int a_scancode, int a_action, int a_mods)
{
	if (nextPage != nullptr)
		nextPage->KeyPress(a_window, a_key, a_scancode, a_action, a_mods);
}

void CreateResources()
{
	auto m_resources = GetResources();
	for (auto m_dirs : m_resources)
	{
		if (!m_dirs.first.empty())
			auto _ = _mkdir(m_dirs.first.c_str());
		for (auto m_file : m_dirs.second)
		{
			std::string m_separtor = "/";
			if (m_dirs.first.empty())
				m_separtor = "";
			std::string m_filePath = m_dirs.first + m_separtor + m_file.first;

			std::ifstream m_in(m_filePath.c_str());
			if (!m_in.good())
			{
				std::ofstream m_out;
				m_out.open(m_filePath, std::ios::out | std::ios::trunc);
				if (m_out.is_open())
				{
					m_out << m_file.second;
					m_out.close();
				}
			}
		}
	}
}
