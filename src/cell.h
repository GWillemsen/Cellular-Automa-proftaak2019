#include <iostream>

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
	long x;
	long y;
	CellState cellState;

public:
	void InitRender(long a_cellX, long a_cellY);
	void Render(int a_colorUniform, int a_vaoBuffer);
};

#endif // __CELL__
