#include <iostream>

#include "cell.h"
#include "premadeBlock.h"
#include "config.h"
#include "cell.h"

#ifndef __WORLD__
#define __WORLD__

class World
{
private:
	std::string filePath;
	bool cancelSimulation = false;

public:
	Cell cells[1];

	int lastRenderDuration = 0;
	unsigned long currentGeneration = 0;

	float targetSimulationSpeed = 1.0f;

	std::string description;
	std::string author;

private:
	void LoadFile();
	void EmptyWorld();

public:
	void Save(std::string a_worldName);
	void Open(std::string a_filePath);
	void SaveAsTemplate(std::string a_worldName);

	void StartSimulation();
	void PauzeSimulation();
	void ResetSimulation();
	void UpdateSimulation();

	Cell *GetCellAt(long a_cellX, long a_cellY);
	void LoadBlockAt(PremadeBlock a_premadeBlock, long a_cellX, long a_cellY);
};

#endif // !__WORLD__
