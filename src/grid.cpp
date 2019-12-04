#include "grid.h"

void Grid::Initialize(Shader a_shader, int a_screenWidth, int a_screenHeight)
{
	this->shader = a_shader;

	this->colorUniform = this->shader.getUniformLocation("u_Color");
	glUniform4f(this->colorUniform, this->lineColor.x, this->lineColor.y, this->lineColor.z, 1.0f);

	// Update the grid lines
	this->Update(a_screenWidth, a_screenHeight);

	// Create a VAO and a VBO
	glGenVertexArrays(1, &this->vaoBuffer);
	glBindVertexArray(this->vaoBuffer);

	glGenBuffers(1, &this->vboBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, this->vboBuffer);
	glBufferData(GL_ARRAY_BUFFER, glm::vec2 * (this->lineAmount / 2), this->lines, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
	glEnableVertexAttribArray(0);
}

void Grid::SetColumnSize(int a_size)
{
	// Make sure that the size is greater then 0
	if (a_size > 0)
		this->colSize = a_size;
}

void Grid::Render()
{
	this->shader.use();

	glBindVertexArray(this->vaoBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, this->vboBuffer);

	glUniform4f(this->colorUniform, this->lineColor.x, this->lineColor.y, this->lineColor.z, 1.0f);
	glDrawArraysInstanced(GL_LINES, 0, this->lineAmount, this->lineAmount);
}

void Grid::Update(int a_screenWidth, int a_screenHeight)
{
	// Calculate the amount of lines the X and Y dimensions need.
	// We add 2 extra lines on each dimension in order to create a smooth scrolling effect.
	int m_horizontalLinesAmount = (a_screenWidth / this->colSize) + 2;
	int m_verticalLinesAmount = (a_screenHeight / this->colSize) + 2;

	// Multiply the total line amount since each line has 2 vertices
	const int totalLineAmount = (m_horizontalLinesAmount + m_verticalLinesAmount) * 2;
	this->lineAmount = totalLineAmount;

	// Update the lines array.
	this->lines = new glm::vec2[totalLineAmount];

	for (int i = 0; i < m_horizontalLinesAmount; i += 2)
	{
		this->lines[i] = glm::vec2(this->colSize * i, 0.0);

		// Make sure that the loop does not get out of bounds
		if (i < totalLineAmount)
			this->lines[i + 1] = glm::vec2(this->colSize * i, (float)(this->colSize * m_horizontalLinesAmount));
	}

	int m_loopOffset = m_horizontalLinesAmount + 1;

	for (int i = 1; i < m_verticalLinesAmount + 1; i += 2)
	{
		this->lines[m_loopOffset + i] = glm::vec2(0.0f, this->colSize * i);

		// Make sure that the loop does not get out of bounds
		if ((m_loopOffset + i) < totalLineAmount)
			this->lines[m_loopOffset + i + 1] = glm::vec2((float)(this->colSize * m_verticalLinesAmount), this->colSize * i);
	}
}

void Grid::UpdateProjectionMatrix(glm::mat4 a_projectionMatrix)
{
	this->projectionMatrix = a_projectionMatrix;
}