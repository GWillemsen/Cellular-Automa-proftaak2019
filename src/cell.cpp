#include "cell.h"
#include "coordinateType.h"

void Cell::InitRender(Shader a_shader)
{
	this->shader = a_shader;
}

void Cell::Render(int a_cellSizeInPx, coordinatePart a_scrollOffsetX, coordinatePart a_scrollOffsetY, glm::vec2* a_offset, glm::vec3* a_color)
{	
	// Give the cell the appropriate color based on the cell state
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
	a_offset->x = (this->x + a_scrollOffsetX) * a_cellSizeInPx;
	a_offset->y = (this->y + a_scrollOffsetY) * a_cellSizeInPx;
}