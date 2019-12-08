#include <iostream>
#include <chrono>
#include <thread>

#include "world.h"
#include "cell.h"
using namespace std;

World world;

Cell* m_cells[] = {
	/*Generator 1*/
	new Cell(0,0, Background),
	new Cell(0,1, Head),
	new Cell(0,2, Tail),
	new Cell(0,3, Conductor),
	new Cell(0,4, Conductor),
	new Cell(0,5, Conductor),
	new Cell(0,6, Conductor),
	new Cell(0,7, Conductor),
	new Cell(0,8, Conductor),
	new Cell(0,9, Background),

	new Cell(1,0, Conductor),
	new Cell(1,1, Background),
	new Cell(1,2, Background),
	new Cell(1,3, Background),
	new Cell(1,4, Background),
	new Cell(1,5, Background),
	new Cell(1,6, Background),
	new Cell(1,7, Background),
	new Cell(1,8, Background),
	new Cell(1,9, Conductor),
	
	new Cell(2,0, Background),
	new Cell(2,1, Conductor),
	new Cell(2,2, Conductor),
	new Cell(2,3, Conductor),
	new Cell(2,4, Conductor),
	new Cell(2,5, Tail),
	new Cell(2,6, Head),
	new Cell(2,7, Conductor),
	new Cell(2,8, Conductor),
	new Cell(2,9, Background),


	/*Generator 2*/
	new Cell(6,0, Background),
	new Cell(6,1, Head),
	new Cell(6,2, Tail),
	new Cell(6,3, Conductor),
	new Cell(6,4, Conductor),
	new Cell(6,5, Conductor),
	new Cell(6,6, Conductor),
	new Cell(6,7, Head),
	new Cell(6,8, Tail),
	new Cell(6,9, Background),

	new Cell(7,0, Conductor),
	new Cell(7,1, Background),
	new Cell(7,2, Background),
	new Cell(7,3, Background),
	new Cell(7,4, Background),
	new Cell(7,5, Background),
	new Cell(7,6, Background),
	new Cell(7,7, Background),
	new Cell(7,8, Background),
	new Cell(7,9, Conductor),
	
	new Cell(8,0, Background),
	new Cell(8,1, Conductor),
	new Cell(8,2, Conductor),
	new Cell(8,3, Conductor),
	new Cell(8,4, Conductor),
	new Cell(8,5, Conductor),
	new Cell(8,6, Conductor),
	new Cell(8,7, Conductor),
	new Cell(8,8, Conductor),
	new Cell(8,9, Background),

	/*XOR*/
	new Cell(3,12, Conductor),
	new Cell(3,13, Conductor),
	new Cell(3,14, Conductor),
	new Cell(3,15, Conductor),
	new Cell(4,12, Conductor),
	new Cell(4,15, Conductor),
	new Cell(5,12, Conductor),
	new Cell(5,13, Conductor),
	new Cell(5,14, Conductor),
	new Cell(5,15, Conductor),

	/*End line*/
	new Cell(4, 16, Conductor),
	new Cell(4, 17, Conductor),
	new Cell(4, 18, Conductor),
	new Cell(4, 19, Conductor),

	/* Connector lines*/
	new Cell(1,10, Conductor),
	new Cell(1,11, Conductor),
	new Cell(1,12, Conductor),
	new Cell(2,13, Conductor),
	
	new Cell(7,10, Conductor),
	new Cell(7,11, Conductor),
	new Cell(7,12, Conductor),
	new Cell(6,13, Conductor),

};

void printCells();

void main()
{
	for (Cell* m_cell : m_cells)
	{
		world.cells.insert(make_pair(make_pair(m_cell->x, m_cell->y), m_cell));
	}
	printCells();
	cout << endl << endl << endl;

	while (true)
	{
		world.UpdateSimulation();
		printCells();
		cout << endl << endl << endl;
		std::this_thread::sleep_for(500ms);
	}
	cin.get();
}

void printCells()
{
	char m_stat[10][20];
	for (int x = 0; x < 10; x++)
	{
		for (int y = 0; y < 20; y++)
		{
			m_stat[x][y] = ' ';
		}
	}

	for (pair<pair<int, int>, Cell*> m_cl : world.cells)
	{
		char m_type = ' ';
		switch (m_cl.second->state)
		{
		case Head:
			m_type = 'H';
			break;
		case Tail:
			m_type = 'T';
			break;
		case Conductor:
			m_type = 'C';
			break;
		default:
			break;
		}
		m_stat[m_cl.second->x][m_cl.second->y] = m_type;
	}
	for (int m_x = 0; m_x < 10; m_x++)
	{
		for (int m_y = 0; m_y < 20; m_y++)
		{
			cout << m_stat[m_x][m_y] << "  ";
		}
		cout << endl;
	}
}
