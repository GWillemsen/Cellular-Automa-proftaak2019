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
#include "simulatorPage.h"
#include "resources.h"

// References.
void framebuffer_size_callback(GLFWwindow* a_window, int a_width, int a_height);
void getError(int a_line);
GLFWwindow* CreateWindow();
void InitGLFW();
bool TryInitGLAD();
unsigned int CreateVBO(float* a_indices, int a_verticeCount);
unsigned int CreateVAO();
unsigned int CreateEBO(unsigned int* indices, int a_indiceCount);

void mouseHover(GLFWwindow* a_window, double a_posX, double a_posY);
void mouseClick(GLFWwindow* a_window, int a_button, int a_action, int a_mods);
void mouseScroll(GLFWwindow* a_window, double a_xOffset, double a_yOffset);

void createResources();

Page* m_nextPage = nullptr;
int screenWidth = 1920;
int screenHeight = 1080;

int main()
{
	createResources();

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

	// Set the viewport
	glViewport(0, 0, screenWidth, screenHeight);

	// Callbacks.

	// Binds the 'framebuffer_size_callback' method to the window resize event.
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// Set callbacks
	glfwSetMouseButtonCallback(window, mouseClick); // Catches the mouse click event
	glfwSetCursorPosCallback(window, mouseHover); // Catches the mouse move event
	glfwSetScrollCallback(window, mouseScroll);

	m_nextPage = new SimulatorPage(window);

	while (m_nextPage)
	{
		Page* m_newNextPage = m_nextPage->Run();
		delete m_nextPage;
		m_nextPage = m_newNextPage;
	}
		
	// Cleanup
	glfwTerminate();
	return 0;
}

// ---
// Callback logic.
// ---

// Handle window resize.
void framebuffer_size_callback(GLFWwindow* a_window, int a_width, int a_height)
{
	m_nextPage->UpdateScreenSize(a_width, a_height);

	// Update the viewport
	glViewport(0, 0, a_width, a_height);
}

void getError(int a_line)
{
	GLenum m_error = glGetError();
	if (m_error != GL_NO_ERROR)
		std::cout << m_error << " At line: " << a_line << std::endl;
}

GLFWwindow* CreateWindow()
{

	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);

	glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
	//glfwWindowHint(GLFW_MAXIMIZED, GLFW_FALSE);
	glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
	
	//GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
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
	// Create a vbo and fill it with data.
	glGenBuffers(1, &vboBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vboBuffer);
	glBufferData(GL_ARRAY_BUFFER, a_verticeCount * sizeof(glm::vec4), a_vertices, GL_STREAM_DRAW);
	return vboBuffer;
}

unsigned int CreateVAO()
{
	unsigned int renderVao;
	// Create a vao.
	glGenVertexArrays(1, &renderVao);
	glBindVertexArray(renderVao);
	return renderVao;
}

void mouseHover(GLFWwindow* a_window, double a_posX, double a_posY)
{
	if (m_nextPage != nullptr)
		m_nextPage->MouseHover(a_window, a_posX, a_posY);
}

void mouseClick(GLFWwindow* a_window, int a_button, int a_action, int a_mods)
{
	if (m_nextPage != nullptr)
		m_nextPage->MouseClick(a_window, a_button, a_action, a_mods);
}

void mouseScroll(GLFWwindow* a_window, double a_xOffset, double a_yOffset)
{
	if (m_nextPage != nullptr)
		m_nextPage->MouseScroll(a_window, a_xOffset, a_yOffset);
}

void createResources()
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
