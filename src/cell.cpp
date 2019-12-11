#include <glad/glad.h>
#include <glm/glm.hpp>
#include "cell.h"

void Cell::InitRender(long a_cellX, long a_cellY)
{
	// Initializes the rendering
}

void Cell::Render(int a_colorUniform, int a_vaoBuffer)
{	
	glBindVertexArray(a_vaoBuffer);

	// Draw the cell
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
}