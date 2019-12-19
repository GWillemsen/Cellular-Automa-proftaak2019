#include "cell.h"

void Cell::InitRender(Shader a_shader, GLuint a_vaoBufferId)
{
	this->shader = a_shader;
	this->vaoBuffer = a_vaoBufferId;
}

void Cell::Render(int a_cellSizeInPx)
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

	glm::mat4 m_translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(a_cellSizeInPx * this->x, a_cellSizeInPx * this->y, 0.0f));
	glm::mat4 m_scaleMatrix = glm::scale(glm::vec3(a_cellSizeInPx, a_cellSizeInPx, 0.0f));

	glm::mat4 m_modelMatrix = m_translationMatrix * m_scaleMatrix;

	// Set the model-view-projection matrix
	glUniformMatrix4fv(m_modelMatrixUniform, 1, GL_FALSE, &m_modelMatrix[0][0]);

	glBindVertexArray(this->vaoBuffer);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);

	glBindVertexArray(0);
}