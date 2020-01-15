#include "cell.h"
#include "coordinateType.h"

void Cell::InitRender(Shader a_shader, GLuint a_vaoBufferId)
{
	this->shader = a_shader;
	this->vaoBuffer = a_vaoBufferId;
}

void Cell::Render(int a_cellSizeInPx, coordinatePart a_scrollOffsetX, coordinatePart a_scrollOffsetY, glm::vec2* a_offset)
{
	// Make sure that there is a valid VAO buffer bound
	if (this->vaoBuffer == -1)
		return;

	this->shader.use();
	int m_modelMatrixUniform = this->shader.getUniformLocation("u_ModelMatrix");
	int m_colorUniform = this->shader.getUniformLocation("u_Color");

	// Give the cell the appropiate color based on the cell state
	switch (this->cellState)
	{
	case CellState::Conductor:
		// Yellow
		glUniform4f(m_colorUniform, 1.0f, 1.0f, 0.0f, 1.0f);
		break;
	case CellState::Head:
		// Red
		glUniform4f(m_colorUniform, 1.0f, 0.0f, 0.0f, 1.0f);
		break;
	case CellState::Tail:
		// Blue
		glUniform4f(m_colorUniform, 0.0f, 0.0f, 1.0f, 1.0f);
		break;
	}

	coordinatePart m_translateX = this->x + a_scrollOffsetX;
	coordinatePart m_translateY = this->y + a_scrollOffsetY;

	glm::mat4 m_translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(a_cellSizeInPx * m_translateX, a_cellSizeInPx * m_translateY, 0.0f));
	glm::mat4 m_scaleMatrix = glm::scale(glm::vec3(a_cellSizeInPx, a_cellSizeInPx, 0.0f));

	glm::mat4 m_modelMatrix = m_translationMatrix * m_scaleMatrix;

	//a_offset->x = 0;
	a_offset->x = (this->x + a_scrollOffsetX) * a_cellSizeInPx;
	//a_offset->y = 0;
	a_offset->y = (this->y + a_scrollOffsetY) * a_cellSizeInPx;

	// Set the model-view-projection matrix
	glUniformMatrix4fv(m_modelMatrixUniform, 1, GL_FALSE, &m_scaleMatrix[0][0]);

	//glBindVertexArray(this->vaoBuffer);
	//glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0, 1);
	
	//glBindVertexArray(0);
}