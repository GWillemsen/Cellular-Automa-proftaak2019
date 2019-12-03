// Include libs.
#include <thread>
#include <iostream>
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
// #include "headers\renderItem\renderItem.h"

int aspectRatio = (640 / 480);

int screenWidth = 800;
int screenHeight = 600;

int curWidth = screenWidth;
int curHeight = screenHeight;

ImVec4 clearColor(0.0f, 0.0f, 0.0f, 1.0f);

// Objects.
Shader basicShaderProgram;

// References.
void framebuffer_size_callback(GLFWwindow* a_window, int a_width, int a_height);
void getError(int a_line);
GLFWwindow* CreateWindow();
void InitGLFW();
bool TryInitGLAD();
unsigned int CreateVBO(float* a_indices, int a_verticeCount);
unsigned int CreateVAO();
unsigned int CreateEBO(unsigned int* indices, int a_indiceCount);

void InitializeRender();
void ProcessInput(GLFWwindow* a_window);
void Update();
void Render();

void UpdateProjectionMatrix();

int main()
{
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

	InitializeRender();

	// TODO: Create a proper timed loop.
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		// Clear the screen.
		glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
		glClear(GL_COLOR_BUFFER_BIT);

		// Handle user input.
		ProcessInput(window);
		Update();
		Render();
		
		glfwSwapBuffers(window);
	}
		
	// Cleanup
	// ImGui_ImplOpenGL3_Shutdown();
	// ImGui_ImplGlfw_Shutdown();
	// ImGui::DestroyContext();

	glfwTerminate();
	return 0;
}

// ---
// Callback logic.
// ---

// Handle window resize.
void framebuffer_size_callback(GLFWwindow* a_window, int a_width, int a_height)
{
	// Update the current width, height and projection matrix
	curWidth = a_width;
	curHeight = a_height;

	UpdateProjectionMatrix();

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
	GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "LearnOpenGL", NULL, NULL);

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

// Application logic

unsigned int vaoBuffer; // Vertex object
unsigned int vboBuffer; // Vertex buffer
unsigned int eboBuffer; // Index buffer
unsigned int shaderProgram; // Default shader

glm::mat4 projectionMatrix;

glm::vec4 vertices[] = {
	// Triangle 1
	glm::vec4(0.0f,  0.0f, 0.0f, 0.0f), // Index 0, Top left
	glm::vec4(0.0f,  100.0f, 0.0f, 0.0f), // Index 1, Bottom left
	glm::vec4(100.0f, 100.0f, 0.0f, 0.0f), // Index 2, Bottom right

	// Triangle 2
	glm::vec4(100.0f,  0.0f, 0.0f, 0.0f)  // Index 3, Top right
};

unsigned int indices[] = {
	// Triangle 1
	0, 1, 2,

	// Triangle 2
	0, 3, 2
};

void InitializeRender()
{
	basicShaderProgram.setVertexShader("shaders/basicVertexShader.glsl");
	basicShaderProgram.setFragmentShader("shaders/basicFragmentShader.glsl");

	shaderProgram = basicShaderProgram.compile();
	basicShaderProgram.use();

	int colorUniform = basicShaderProgram.getUniformLocation("u_Color");
	glUniform4f(colorUniform, 1.0f, 0.2f, 0.3f, 1.0f);
	getError(__LINE__);

	// Create a coordinate system that starts in the lower left corner
	// ALWAYS USE FLOATS IN A PROJECTION MATRIX
	projectionMatrix = glm::ortho(0.0f, (float)screenWidth, 0.0f, (float)screenHeight, -1.0f, 1.0f);
	UpdateProjectionMatrix();

	vaoBuffer = CreateVAO();
	vboBuffer = CreateVBO((float*)vertices, 4);
	eboBuffer = CreateEBO(indices, 6);
	getError(__LINE__);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);
	glEnableVertexAttribArray(0);
	getError(__LINE__);
}

void ProcessInput(GLFWwindow* a_window)
{
	// If the escape key gets pressed, close the window.
	if (glfwGetKey(a_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(a_window, true);
}

void Update()
{

}

void Render()
{
	basicShaderProgram.use();
	glBindVertexArray(vaoBuffer);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
	getError(__LINE__);
}

void UpdateProjectionMatrix()
{
	// Update the scaling within the projection matrix if the viewport changes
	if (curWidth != screenWidth || curHeight != screenHeight)
	{
		// Get the scale between the original and current sizes
		//double scalarX = screenWidth / curWidth;
		double scalar = (screenHeight / curHeight) != 0 ? (screenHeight / curHeight) : (curHeight / screenHeight);
		
		std::cout << scalar << std::endl;

		//std::cout << "scaleX: " << scalarX << " scaleY: " << scalarY << std::endl;

		// Update the projection matrix and apply the scales
		projectionMatrix = glm::ortho(0.0f, (float)(curWidth * scalar), 0.0f, (float)(curHeight* scalar), -1.0f, 1.0f);
	}

	int matrixUniform = basicShaderProgram.getUniformLocation("u_Projection");
	glUniformMatrix4fv(matrixUniform, 1, GL_FALSE, &projectionMatrix[0][0]);
	getError(__LINE__);
}