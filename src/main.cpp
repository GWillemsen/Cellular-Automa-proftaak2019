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

int screenWidth = 800;
int screenHeight = 600;

// Objects.
Shader basicShaderProgram;

// References.
void framebuffer_size_callback(GLFWwindow* a_window, int a_width, int a_height);
void processInput(GLFWwindow* a_window);
void getError(int a_line);
void RenderImgui(bool &show_demo_window, bool &show_another_window, ImVec4 &clear_color);
GLFWwindow* CreateWindow();
void InitGLFW();
unsigned int CreateVBO(float* a_indices, int a_verticeCount);
unsigned int CreateVAO();
unsigned int CreateEBO(unsigned int* indices, int a_indiceCount);
void UpdateClearColor(ImVec4& a_clear_color, float a_deltaSeconds);
void NormalizeVector(ImVec4& a_vector);
void UpdateRotation(glm::vec3* data, glm::vec3* a_vertices, int a_verticesCount, float a_addedRotation);
bool TryInitGLAD();

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

	glm::vec3 vertices[] = {
		// Triangle 1
		glm::vec3(-0.5f,  0.5f, 0.0f), // Index 0, Top left
		glm::vec3(-0.5f, -0.5f, 0.0f), // Index 1, Bottom left
		glm::vec3(0.5f, -0.5f, 0.0f), // Index 2, Bottom right

		// Triangle 2
		glm::vec3(0.5f,  0.5f, 0.0f)  // Index 3, Top right
	};

	unsigned int indices[] = {
		// Triangle 1
		0, 1, 2,

		// Triangle 2
		0, 3, 2
	};

	// Rendering steps:
	// 1. Create a vao with glGenVertexArrays and then bind it with glBindVertexArray.
	// 2. Create a vbo.
	// 3. Set vertex attribute pointers.
	// 4. enable vertex attribute pointers.
	// 5. Render.

	unsigned int renderVao = CreateVAO();
	unsigned int vboBuffer = CreateVBO((float*)vertices, 4);
	unsigned int eboBuffer = CreateEBO(indices, 6);
	getError(118);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(0);


	const char* glsl_version = "#version 130";

	IMGUI_CHECKVERSION();

	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	
	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer bindings

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.0F, 0.00f, 1.00f);

	// Application Loop.
	float deltaseconds = 0;
	auto lastTime = std::chrono::high_resolution_clock::now();
	float rotation = 0.0f;
	while (!glfwWindowShouldClose(window))
	{
		auto currentTime = std::chrono::high_resolution_clock::now();
		auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime).count();
		deltaseconds = diff / 1000.0f;
		lastTime = currentTime;

		glfwPollEvents();
		
		//glClearColor(1 - clear_color.x, 1 - clear_color.y, 1 - clear_color.z, 1 - clear_color.w);
		glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	
		getError(167);
		glm::vec3 *data = (glm::vec3 *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
		UpdateRotation(data, vertices, sizeof(vertices) / sizeof(glm::vec3), rotation);
		glUnmapBuffer(GL_ARRAY_BUFFER);
		
		if (rotation > 360)
			rotation = 0;
		rotation += 10;

		//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		processInput(window);

		UpdateClearColor(clear_color, deltaseconds * 2);

		// Start rendering.
		basicShaderProgram.use();
		glUniform4f(colorUniform, clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glBindVertexArray(renderVao);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
		RenderImgui(show_demo_window, show_another_window, clear_color);



		// Rendering

		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		
		// use the direct pointer from the buffer map. then use the original vertice array to get its size.
		glfwSwapBuffers(window);
	}
		
	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

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

void RenderImgui(bool &a_show_demo_window, bool &a_show_another_window, ImVec4 &a_clear_color)
{

	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	if (a_show_demo_window)
		ImGui::ShowDemoWindow(&a_show_demo_window);

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
	{
		static float f = 0.0f;
		static int counter = 0;

		ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

		ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		ImGui::Checkbox("Demo Window", &a_show_demo_window);      // Edit bools storing our window open/close state
		ImGui::Checkbox("Another Window", &a_show_another_window);

		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::ColorEdit4("clear color", (float*)&a_clear_color); // Edit 3 floats representing a color

		if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}

	// 3. Show another simple window.
	if (a_show_another_window)
	{
		ImGui::Begin("Another Window", &a_show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui::Text("Hello from another window!");
		if (ImGui::Button("Close Me"))
			a_show_another_window = false;
		ImGui::End();
	}
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
	glBufferData(GL_ARRAY_BUFFER, a_verticeCount * sizeof(glm::vec3), a_vertices, GL_STREAM_DRAW);
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

void UpdateClearColor(ImVec4& a_clear_color, float a_deltaSeconds)
{
	// clear_color
	/*
		x = red
		y = green
		z = blue
		w = alpha
	*/
	NormalizeVector(a_clear_color);

	if (a_clear_color.x < 1.0f && a_clear_color.y == 0.0f && a_clear_color.z == 0.0f)
	{
		a_clear_color.x += a_deltaSeconds;
	}
	else if (a_clear_color.x > 0.0f && a_clear_color.y < 1.0f && a_clear_color.z == 0.0f)
	{
		a_clear_color.y += a_deltaSeconds;

		a_clear_color.x -= a_deltaSeconds;
		if (a_clear_color.x <= 0.01f)
		{
			a_clear_color.x = 0.0f;
		}
	}
	else if (a_clear_color.x == 0.0f && a_clear_color.y > 0.0f && a_clear_color.z < 1.0f)
	{
		a_clear_color.z += a_deltaSeconds;

		a_clear_color.y -= a_deltaSeconds;
		if (a_clear_color.y <= 0.01f)
		{
			a_clear_color.y = 0.0f;
		}
	}
	else if (a_clear_color.x >= 0.0f && a_clear_color.y == 0.0f && a_clear_color.z > 0.0f)
	{
		a_clear_color.x += a_deltaSeconds;

		a_clear_color.z -= a_deltaSeconds;
		if (a_clear_color.z <= 0.01f)
		{
			a_clear_color.z = 0.0f;
		}
	}
	NormalizeVector(a_clear_color);
}

void NormalizeVector(ImVec4& a_vector)
{
	if (a_vector.x > 1.00000F)
		a_vector.x = 1.0F;

	if (a_vector.y > 1.00000F)
		a_vector.y = 1.0F;

	if (a_vector.z > 1.00000F)
		a_vector.z = 1.0F;

	if (a_vector.w > 1.00000F)
		a_vector.w = 1.0F;

	if (a_vector.x < 0)
		a_vector.x = 0;
	if (a_vector.y < 0)
		a_vector.y = 0;
	if (a_vector.z < 0)
		a_vector.z = 0;
	if (a_vector.w < 0)
		a_vector.w = 0;
}

void UpdateRotation(glm::vec3* data, glm::vec3* a_vertices, int a_verticesCount, float a_addedRotation)
{
	/*
	mat4  -> 3 dimensions + 1 (it was there for a reason, just don't remember any more)
	a_addedRotation -> conversion to radians as that is format for glm::rotate
	vec3 -> dimensions of rotations, ie all dims with 1 in them will have the rotation applied to them, 0 means not.
	*/
	glm::mat4 identity = glm::mat4(1.0f);
	glm::mat4 rotation = glm::rotate(identity, glm::radians(a_addedRotation), glm::vec3(0.0f, -0.0f, -1.0f));
	
	glm::vec4 a = identity * glm::vec4(5);

	while (a_verticesCount--)
	{
		glm::vec4 calcVec = glm::vec4(a_vertices->x, a_vertices->y, a_vertices->z, 1.0f);
		glm::vec3 newVec = glm::vec3(rotation * calcVec);
		*data = newVec;
		a_vertices++;
		data++;
	}
}

