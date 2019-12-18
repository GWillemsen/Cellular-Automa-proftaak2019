#include <glad/glad.h>
#include <glm/glm.hpp>
#include "cell.h"

void Cell::InitRender(GLint64 a_cellX, GLint64 a_cellY)
{
	// Initializes the rendering

	this->x = a_cellX;
	this->y = a_cellY;
}

void Cell::Render(int a_colorUniform, int a_vaoBuffer)
{
	glBindVertexArray(a_vaoBuffer);

	// Draw the cell
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
}