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
	
	unsigned char neighborCount;
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
	}

	Cell()
	{
		this->decayState = Background;
		this->cellState = Background;
		this->x = 0;
		this->y = 0;
		this->atomic_neighborCount.store(0);
	}

	void InitRender(Shader a_shader, GLuint a_vaoBufferId);
	void Render(int a_cellSizeInPx, long a_scrollOffsetX, long a_scrollOffsetY);
};

#endif // __CELL__
