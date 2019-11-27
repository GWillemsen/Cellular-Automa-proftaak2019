// Include libs.
#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <math.h>
#include <chrono>

#include "shader.h"
// #include "headers\renderItem\renderItem.h"

// Includes the standard "std" lib
#include <iostream>

int screenWidth = 800;
int screenHeight = 600;

// Objects.
Shader basicShaderProgram;

// References.
void framebuffer_size_callback(GLFWwindow* a_window, int a_width, int a_height);
void processInput(GLFWwindow* a_window);
void getError(int a_line);

int main()
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

	// ---
	// Create Window.
	// ---

	// Instantiate the window object.
	GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "LearnOpenGL", NULL, NULL);

	// Make sure that the window is created.
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window." << std::endl;
		glfwTerminate();

		std::cin.get();
		return -1;
	}

	glfwMakeContextCurrent(window);

	// ---
	// Initialize GLAD.
	// ---

	// Make sure that glad has been initialized successfully.
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD." << std::endl;

		std::cin.get();
		return -1;
	}

	// ---
	// Set the viewport
	// ---

	glViewport(0, 0, screenWidth, screenHeight);

	// ---
	// Setup callbacks.
	// ---

	// Binds the 'framebuffer_size_callback' method to the window resize event.
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// ---
	// Setup rendering.
	// ---

	unsigned int shaderProgram;

	basicShaderProgram.setVertexShader("shaders/basicVertexShader.glsl");
	basicShaderProgram.setFragmentShader("shaders/basicFragmentShader.glsl");

	shaderProgram = basicShaderProgram.compile();
	basicShaderProgram.use();

	// Set the rectangle color.
	int colorUniform = basicShaderProgram.getUniformLocation("u_Color");
	glUniform4f(colorUniform, 1.0f, 0.2f, 0.3f, 1.0f);

	float vertices[] = {
		// Triangle 1
		-0.5f,  0.5f, 0.0f, // Index 0, Top left
		-0.5f, -0.5f, 0.0f, // Index 1, Bottom left
		 0.5f, -0.5f, 0.0f, // Index 2, Bottom right

		 // Triangle 2
		 0.5f,  0.5f, 0.0f  // Index 3, Top right
	};

	unsigned int indices[] = {
		// Triangle 1
		0, 1, 2,

		// Triangle 2
		0, 3, 2
	};

	unsigned int renderVao;
	unsigned int vboBuffer;
	unsigned int eboBuffer;

	// Rendering steps:
	// 1. Create a vao with glGenVertexArrays and then bind it with glBindVertexArray.
	// 2. Create a vbo.
	// 3. Set vertex attribute pointers.
	// 4. enable vertex attribute pointers.
	// 5. Render.

	// Create a vao.
	glGenVertexArrays(1, &renderVao);
	glBindVertexArray(renderVao);

	// Create a vbo and fill it with data.
	glGenBuffers(1, &vboBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vboBuffer);
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &eboBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Keeps track when we can cycle the colors.
	int colorCycle = 0;
	int colorCycleTick = 0;

	// Controls the color of the rectangle.
	float redColorChannel = 0.0f;
	float greenColorChannel = 0.0f;
	float blueColorChannel = 0.0f;

	// Application Loop.
	float deltaseconds = 0;
	auto lastTime = std::chrono::high_resolution_clock::now();
	while (!glfwWindowShouldClose(window))
	{
		auto currentTime = std::chrono::high_resolution_clock::now();
		auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime).count();
		deltaseconds = diff / 10000.0f;
		lastTime = currentTime;

		glfwPollEvents();
		processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		float valueDiff = deltaseconds * 10;

		if (redColorChannel > 1.00000F)
			redColorChannel = 1.0F;

		if (greenColorChannel > 1.00000F)
			greenColorChannel = 1.0F;

		if (blueColorChannel > 1.00000F)
			blueColorChannel = 1.0F;

		if (colorCycleTick >= colorCycle)
		{
			if (redColorChannel < 1.0f && greenColorChannel == 0.0f && blueColorChannel == 0.0f)
			{
				redColorChannel += valueDiff;
			}
			else if (redColorChannel > 0.0f && greenColorChannel < 1.0f && blueColorChannel == 0.0f)
			{
				greenColorChannel += valueDiff;

				redColorChannel -= valueDiff;
				if (redColorChannel <= 0.01f)
				{
					redColorChannel = 0.0f;
				}
			}
			else if (redColorChannel == 0.0f && greenColorChannel > 0.0f && blueColorChannel < 1.0f)
			{
				blueColorChannel += valueDiff;

				greenColorChannel -= valueDiff;
				if (greenColorChannel <= 0.01f)
				{
					greenColorChannel = 0.0f;
				}
			}
			else if (redColorChannel >= 0.0f && greenColorChannel == 0.0f && blueColorChannel > 0.0f)
			{
				redColorChannel += valueDiff;

				blueColorChannel -= valueDiff;
				if (blueColorChannel <= 0.01f)
				{
					blueColorChannel = 0.0f;
				}
			}

			//std::cout << redColorChannel << ", " << greenColorChannel << ", " << blueColorChannel << std::endl;


			colorCycleTick = 0;
		}
		else
		{
			colorCycleTick += 1;
		}

		// Start rendering.
		basicShaderProgram.use();
		glUniform4f(colorUniform, redColorChannel, greenColorChannel, blueColorChannel, 1.0f);
		glBindVertexArray(renderVao);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
		// End rendering.

		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}

// ---
// Callback logic.
// ---

// Handle window resize.
void framebuffer_size_callback(GLFWwindow* a_window, int a_width, int a_height)
{
	glViewport(0, 0, a_width, a_height);
}

void processInput(GLFWwindow* a_window)
{
	// If the escape key gets pressed, close the window.
	if (glfwGetKey(a_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(a_window, true);
}

void getError(int a_line)
{
	GLenum m_error = glGetError();

	if (m_error != GL_NO_ERROR)
		std::cout << m_error << " At line: " << a_line << std::endl;
}