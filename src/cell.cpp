#include "cell.h"
#include "coordinateType.h"

void Cell::InitRender(Shader a_shader, GLuint a_vaoBufferId)
{
	this->shader = a_shader;
	this->vaoBuffer = a_vaoBufferId;
}

void Cell::Render(int a_cellSizeInPx, coordinatePart a_scrollOffsetX, coordinatePart a_scrollOffsetY, glm::vec2* a_offset, glm::vec3* a_color)
{
	// Make sure that there is a valid VAO buffer bound
	if (this->vaoBuffer == -1)
		return;

	this->shader.use();
	int m_modelMatrixUniform = this->shader.getUniformLocation("u_ModelMatrix");
	
	// Give the cell the appropiate color based on the cell state
	switch (this->cellState)
	{
	case CellState::Conductor:
		// Yellow
		*a_color = glm::vec3(1.0f, 1.0f, 0.0f);
		break;
	case CellState::Head:
		// Red
		*a_color = glm::vec3(1.0f, 0.0f, 0.0f);
		break;
	case CellState::Tail:
		// Blue
		*a_color = glm::vec3(0.0f, 0.0f, 1.0f);
		break;
	}

	glm::mat4 m_scaleMatrix = glm::scale(glm::vec3(a_cellSizeInPx, a_cellSizeInPx, 0.0f));

	a_offset->x = (this->x + a_scrollOffsetX) * a_cellSizeInPx;
	a_offset->y = (this->y + a_scrollOffsetY) * a_cellSizeInPx;

	// Set the model-view-projection matrix
	glUniformMatrix4fv(m_modelMatrixUniform, 1, GL_FALSE, &m_scaleMatrix[0][0]);

}