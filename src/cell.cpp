/*
MIT License 

Copyright (c) 2020 Guylian Gilsing & Giel Willemsen

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

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