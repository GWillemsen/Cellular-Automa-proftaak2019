#include <iostream>
#include <map>
#include <vector>
#include <condition_variable>

#include "cell.h"
#include "premadeBlock.h"
#include "config.h"
#include "cell.h"

#ifndef __WORLD__
#define __WORLD__



class World
{

private:
	class ThreadCombo {
		public:
			std::mutex lock;
			std::condition_variable cv;
			unsigned long current_generation;
			ThreadCombo()
			{
				this->current_generation = 0;
			};
	};

	std::string filePath;
	bool cancelSimulation = false;
	std::mutex mt_generation;
	std::vector<std::thread> vec_simUpdaters;
	std::condition_variable cv_nextUpdate;
	std::map<int, ThreadCombo*> threadComboData;
	int totalThreads;
	std::mutex lastPartLock;
	std::condition_variable lastPartGenerationCv;
	unsigned long lastPartGeneration;
	std::thread lastPartProcessor;
public:
	std::map<std::pair<int, int>, Cell*> cells;

	int lastRenderDuration = 0;
	unsigned long currentGeneration = 0;

	float targetSimulationSpeed = 1.0f;

	std::string description;
	std::string author;

private:
	void LoadFile();
	void EmptyWorld();
	void IncrementNeighbors(int x, int y);
	void IncrementNeighborsOld(int x, int y);
	void ProcessPartContinuesly(int a_threadId, int a_maxThreads);
	void ProcessLastPart();
public:
	World() : World(false) {
		
	}
	World(bool a_mulithreaded);
	void UpdateSimulationContinuesly();
	void UpdateSimulationSerial();
	void Save(std::string a_worldName);
	void Open(std::string a_filePath);
	void SaveAsTemplate(std::string a_worldName);

	void StartSimulation();
	void PauzeSimulation();
	void ResetSimulation();

	Cell *GetCellAt(long a_cellX, long a_cellY);
	void LoadBlockAt(PremadeBlock a_premadeBlock, long a_cellX, long a_cellY);
};

#endif // !__WORLD__
