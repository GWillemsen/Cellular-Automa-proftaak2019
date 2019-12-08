#include <iostream>
#include <mutex>
#include <glad/glad.h>

#ifndef __CELL__
#define __CELL__

enum CellState
{
	Head = 0,
	Tail = 1,
	Conductor = 2,
	Background = 3
};

class Cell
{
public:
	GLint64 x;
	GLint64 y;
	CellState state;
	CellState decayState;
	unsigned char neighorCount;

public:
	void InitRender(long a_cellX, long a_cellY);
	void Render();
	Cell(GLint64 x, GLint64 y, CellState state) : Cell() {
		this->x = x;
		this->y = y;
		this->state = state;
		this->decayState = state;
	}
	Cell() {
		this->decayState = Background;
		this->state = Background;
		this->x = 0;
		this->y = 0;
		this->neighorCount = 0;
	}
};

#endif // __CELL__
