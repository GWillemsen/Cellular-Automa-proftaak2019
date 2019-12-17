#include <iostream>
#include <atomic>
#include <glad/glad.h>

#ifndef __CELL__
#define __CELL__

enum CellState
{
	Conductor = 0,
	Head = 1,
	Tail = 2,
	Background = 3
};

class Cell
{
public:
	GLint64 x;
	GLint64 y;
	CellState cellState;
	CellState decayState;
	unsigned char neighborCount;
	std::atomic<unsigned char> atomic_neighborCount;
public:
	void InitRender(long a_cellX, long a_cellY);
	void Render(int a_colorUniform, int a_vaoBuffer);
	Cell(GLint64 x, GLint64 y, CellState state) : Cell() {
		this->x = x;
		this->y = y;
		this->cellState = state;
		this->decayState = state;
	}
	Cell() {
		this->decayState = Background;
		this->cellState = Background;
		this->x = 0;
		this->y = 0;
		this->atomic_neighborCount.store(0);
	}
};

#endif // __CELL__
