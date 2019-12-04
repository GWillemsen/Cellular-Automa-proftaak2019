#include <iostream>

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
	long x;
	long y;
	CellState cellState;

public:
	void InitRender(long a_cellX, long a_cellY);
	void Render();
};

#endif // __CELL__
