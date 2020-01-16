#include "cell.h"
#include "coordinateType.h"
#include "config.h"

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
		*a_color = Config::instance->conductorColor;
		break;
	case CellState::Head:
		*a_color = Config::instance->headColor;
		break;
	case CellState::Tail:
		*a_color = Config::instance->tailColor;
		break;
	}

	a_offset->x = (this->x + a_scrollOffsetX) * a_cellSizeInPx;
	a_offset->y = (this->y + a_scrollOffsetY) * a_cellSizeInPx;
}