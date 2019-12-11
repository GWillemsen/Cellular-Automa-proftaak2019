#include <iostream>
#include <chrono>
#include <thread>
#include <time.h>

#include "world.h"
#include "cell.h"
#define SHOW_TYPE 2

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

void printCells(World const& world);
void addGenerator(int x, int y, World& world);
void cleanUp();
void cleanUpWorld(World& a_world);
bool compareWorlds(World const& a_baseWorld, World const& a_compare);

void main()
{
#if SHOW_TYPE == 1
	for (Cell* m_cell : m_cells)
	{
		world.cells.insert(make_pair(make_pair(m_cell->x, m_cell->y), m_cell));
	}

	while (true)
	{
		world.UpdateSimulationContinuesly();
		printCells();
		cout << endl << endl << endl;
		std::this_thread::sleep_for(500ms);
		cin.get();
	}

#elif SHOW_TYPE == 2

	World mulithreadWorld (true);
	World asyncMulithreadWorld;
	World serialWorld;
	int xWidth = 5;
	int yHeight = 11;
	int xCount = 3000;
	int yCount = 200;
	int generatorCount = 0;
	std::cout << "Generating world" << std::endl;

	for (int x = 0; x < xCount; x += xWidth)
	{
		for (int y = 0; y < yCount; y += yHeight)
		{
			generatorCount++;
			addGenerator(x, y, mulithreadWorld);
			addGenerator(x, y, asyncMulithreadWorld);
			addGenerator(x, y, serialWorld);
		}
	}
	
	int m_testGenerationCount = 100;

	clock_t start_multi = clock();
	for (int cnt = 0; cnt < m_testGenerationCount; cnt++)
	{
		mulithreadWorld.UpdateSimulationContinuesly();
	}
	clock_t end_multi = clock();
	clock_t start_async = clock();
	for (int cnt = 0; cnt < m_testGenerationCount; cnt++)
	{
		asyncMulithreadWorld.UpdateSimulationAsync();
	}
	clock_t end_async = clock();
	clock_t start_serial = clock();
	for (int cnt = 0; cnt < m_testGenerationCount; cnt++)
	{
		serialWorld.UpdateSimulationSerial();
	}
	clock_t end_serial = clock();

	double ms_multi = (end_multi - start_multi) / (CLOCKS_PER_SEC / 1000);
	double ms_async = (end_async - start_async) / (CLOCKS_PER_SEC / 1000);
	double ms_serial = (end_serial - start_serial) / (CLOCKS_PER_SEC / 1000);
	std::cout << "It took: " << ms_multi << " ms with continued threading. Thus " << ms_multi / m_testGenerationCount << " per cycle" << std::endl;
	std::cout << "It took: " << ms_async << " ms with async threading. Thus " << ms_async / m_testGenerationCount << " per cycle" << std::endl;
	std::cout << "It took: " << ms_serial << " ms with serial. Thus " << ms_serial / m_testGenerationCount << " per cycle" << std::endl;
	printCells(mulithreadWorld);

	std::cout << "Serial as base compared with multi thread: " << compareWorlds(serialWorld, mulithreadWorld) << std::endl;
	std::cout << "Serial as base compared with async: " << compareWorlds(serialWorld, asyncMulithreadWorld) << std::endl;
	std::cout << "Multi threaded as base compared with async: " << compareWorlds(mulithreadWorld, asyncMulithreadWorld) << std::endl;

	cleanUpWorld(mulithreadWorld);
	cleanUpWorld(asyncMulithreadWorld);
	cleanUpWorld(serialWorld);
#elif SHOW_TYPE == 3
	int xWidth = 5;
	int yHeight = 11;
	int xCount = 30;
	int yCount = 20;
	int generatorCount = 0;
	cout << "Generating world" << endl;

	for (int x = 0; x < xCount; x += xWidth)
	{
		for (int y = 0; y < yCount; y += yHeight)
		{
			generatorCount++;
			addGenerator(x, y, world);
		}
	}
	cout << "Generated " << generatorCount << " generators" << endl;

	//printCells();
	cout << endl << endl << endl;
	int cycleCount = 1 * 1;
	clock_t start_time = clock();
	for (int i = 0; i < cycleCount; i++)
	{
		world.UpdateSimulationContinuesly();
		if (i % 10 == 0)
			cout << "At: " << i << endl;
	}
	clock_t end = clock();
	double ms = (end - start_time) / (CLOCKS_PER_SEC / 1000);
	cout << "It took: " << ms << " ms with continued threading. Thus " << ms / cycleCount << " per cycle" << endl;

	start_time = clock();
	for (int i = 0; i < cycleCount; i++)
	{
		world.UpdateSimulationAsync();
		if (i % 10 == 0)
			cout << "At: " << i << endl;
	}
	end = clock();
	ms = (end - start_time) / (CLOCKS_PER_SEC / 1000);
	cout << "It took: " << ms << " ms with std::async. Thus " << ms / cycleCount << " per cycle" << endl;

	start_time = clock();
	for (int i = 0; i < cycleCount; i++)
	{
		world.UpdateSimulationSerial();
		if (i % 10 == 0)
			cout << "At: " << i << endl;
	}
	end = clock();
	ms = (end - start_time) / (CLOCKS_PER_SEC / 1000);
	cout << "It took: " << ms << " ms single threaded. Thus " << ms / cycleCount << " per cycle" << endl;
#endif

	cleanUp();
	std::cin.get();
}

void cleanUp()
{
	for (auto m_pair : world.cells)
	{
		delete m_pair.second;
	}
	world.cells.clear();

	for (auto m_cell : m_cells)
	{
		delete m_cell;
	}
}

void printCells(World const& world)
{
	const int width = 10;
	const int height = 10;
	char m_stat[width][height];
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			m_stat[x][y] = ' ';
		}
	}

	for (std::pair<std::pair<int, int>, Cell*> m_cl : world.cells)
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
		if (m_cl.second->x < width && m_cl.second->y < height)
			m_stat[m_cl.second->x][m_cl.second->y] = m_type;
	}
	std::string data = "";
	for (int m_x = 0; m_x < width; m_x++)
	{
		for (int m_y = 0; m_y < height; m_y++)
		{
			data += m_stat[m_x][m_y];
			data.append("  ");
		}
		data.append("\r\n");
	}
	std::cout << data;
}

void addGenerator(int x, int y, World& world)
{
	using std::make_pair;
	#define active
	world.cells.insert(make_pair(make_pair(x + 0, y + 0), new Cell(x + 0, y + 0, Background)));		
#ifdef active
	world.cells.insert(make_pair(make_pair(x + 0, y + 1), new Cell(x + 0, y + 1, Head)));
	world.cells.insert(make_pair(make_pair(x + 0, y + 2), new Cell(x + 0, y + 2, Tail)));
#else
	world.cells.insert(make_pair(make_pair(x + 0, y + 1), new Cell(x + 0, y + 1, Conductor)));
	world.cells.insert(make_pair(make_pair(x + 0, y + 2), new Cell(x + 0, y + 2, Conductor)));
#endif // active
	world.cells.insert(make_pair(make_pair(x + 0, y + 3), new Cell(x + 0, y + 3, Conductor)));
	world.cells.insert(make_pair(make_pair(x + 0, y + 4), new Cell(x + 0, y + 4, Conductor)));
	world.cells.insert(make_pair(make_pair(x + 0, y + 5), new Cell(x + 0, y + 5, Conductor)));
	world.cells.insert(make_pair(make_pair(x + 0, y + 6), new Cell(x + 0, y + 6, Conductor)));
	world.cells.insert(make_pair(make_pair(x + 0, y + 7), new Cell(x + 0, y + 7, Conductor)));
	world.cells.insert(make_pair(make_pair(x + 0, y + 8), new Cell(x + 0, y + 8, Conductor)));
	world.cells.insert(make_pair(make_pair(x + 0, y + 9), new Cell(x + 0, y + 9, Background)));

	world.cells.insert(make_pair(make_pair(x + 1, y + 0), new Cell(x + 1, y + 0, Conductor)));
	world.cells.insert(make_pair(make_pair(x + 1, y + 1), new Cell(x + 1, y + 1, Background)));
	world.cells.insert(make_pair(make_pair(x + 1, y + 2), new Cell(x + 1, y + 2, Background)));
	world.cells.insert(make_pair(make_pair(x + 1, y + 3), new Cell(x + 1, y + 3, Background)));
	world.cells.insert(make_pair(make_pair(x + 1, y + 4), new Cell(x + 1, y + 4, Background)));
	world.cells.insert(make_pair(make_pair(x + 1, y + 5), new Cell(x + 1, y + 5, Background)));
	world.cells.insert(make_pair(make_pair(x + 1, y + 6), new Cell(x + 1, y + 6, Background)));
	world.cells.insert(make_pair(make_pair(x + 1, y + 7), new Cell(x + 1, y + 7, Background)));
	world.cells.insert(make_pair(make_pair(x + 1, y + 8), new Cell(x + 1, y + 8, Background)));
	world.cells.insert(make_pair(make_pair(x + 1, y + 9), new Cell(x + 1, y + 9, Conductor)));

	world.cells.insert(make_pair(make_pair(x + 2, y + 0), new Cell(x + 2, y + 0, Background)));
	world.cells.insert(make_pair(make_pair(x + 2, y + 1), new Cell(x + 2, y + 1, Conductor)));
	world.cells.insert(make_pair(make_pair(x + 2, y + 2), new Cell(x + 2, y + 2, Conductor)));
	world.cells.insert(make_pair(make_pair(x + 2, y + 3), new Cell(x + 2, y + 3, Conductor)));
	world.cells.insert(make_pair(make_pair(x + 2, y + 4), new Cell(x + 2, y + 4, Conductor)));
#ifdef active
	world.cells.insert(make_pair(make_pair(x + 2, y + 5), new Cell(x + 2, y + 5, Tail)));
	world.cells.insert(make_pair(make_pair(x + 2, y + 6), new Cell(x + 2, y + 6, Head)));
#else
	world.cells.insert(make_pair(make_pair(x + 2, y + 5), new Cell(x + 2, y + 5, Conductor)));
	world.cells.insert(make_pair(make_pair(x + 2, y + 6), new Cell(x + 2, y + 6, Conductor)));
#endif
	world.cells.insert(make_pair(make_pair(x + 2, y + 7), new Cell(x + 2, y + 7, Conductor)));
	world.cells.insert(make_pair(make_pair(x + 2, y + 8), new Cell(x + 2, y + 8, Conductor)));
	world.cells.insert(make_pair(make_pair(x + 2, y + 9), new Cell(x + 2, y + 9, Background)));
}

void cleanUpWorld(World& a_world)
{
	for (auto m_cell : a_world.cells)
	{
		delete m_cell.second;
	}
	a_world.cells.clear();
}

bool compareWorlds(World const& a_baseWorld, World const& a_compare)
{
	bool m_noMatch = false;
	for (auto m_block : a_baseWorld.cells)
	{
		auto m_foundPair = a_compare.cells.find(std::make_pair(m_block.second->x, m_block.second->y));
		if (m_foundPair == a_compare.cells.end())
		{
			m_noMatch = true;
			break;
		}
		else
		{
			if (m_foundPair->second->state != m_block.second->state)
			{
				m_noMatch = true;
				break;
			}
		}
	}
	return m_noMatch;
}