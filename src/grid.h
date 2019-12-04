//#include "shader.h"
#include <GLAD/glad.h>
#include <GLM/glm.hpp>
#include <GLM/ext/matrix_projection.hpp>
#include <glm/ext/matrix_clip_space.hpp> // glm::perspective

class Grid
{
private:
	int colSize = 32;

	glm::vec2* lines;
	glm::vec3 lineColor = glm::vec3(0.4f, 0.4f, 0.4f);
	int lineAmount = 0;

	Shader shader;
	int colorUniform;
	glm::mat4 projectionMatrix;

	unsigned int vaoBuffer;
	unsigned int vboBuffer;

public:
	void Initialize(Shader a_shader, int a_screenWidth, int a_screenHeight);
	void Update(int a_screenWidth, int a_screenHeight);
	void SetColumnSize(int a_size);
	void Render();
	void UpdateProjectionMatrix(glm::mat4 a_projectionMatrix);
	void SetGridColor(glm::vec3 a_color);
};