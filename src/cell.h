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
#include <iostream>
#include <atomic>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm\gtx\transform.hpp>

#include "shader.h"
#include "coordinateType.h"

#ifndef __CELL__
#define __CELL__

enum CellState : int
{
	Conductor = 0,
	Head = 1,
	Tail = 2,
	Background = 3
};

class Cell
{
public:
	coordinatePart x;
	coordinatePart y;

	CellState cellState;
	CellState decayState;
	
	std::atomic<unsigned char> atomic_neighborCount;

private:
	GLuint vaoBuffer = -1;
	Shader shader;

public:
	Cell(coordinatePart x, coordinatePart y, CellState state) : Cell()
	{
		this->x = x;
		this->y = y;
		this->cellState = state;
		this->decayState = state;
		this->shader = Shader();
		this->atomic_neighborCount.store(0);
	}

	Cell() : shader()
	{
		this->decayState = Background;
		this->cellState = Background;
		this->x = 0;
		this->y = 0;
		this->atomic_neighborCount.store(0);
	}

	void InitRender(Shader a_shader);
	void Render(int a_cellSizeInPx, coordinatePart a_scrollOffsetX, coordinatePart a_scrollOffsetY, glm::vec2* a_offset, glm::vec3* a_color);
};

#endif // __CELL__
