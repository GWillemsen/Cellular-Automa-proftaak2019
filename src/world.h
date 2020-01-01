#include <iostream>
#include <map>
#include <functional> // create your own lambda
#include <vector>
#include <condition_variable>
#include <shared_mutex>

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

	std::thread timerThread;
	std::mutex simCalcUpdateLock;
	std::condition_variable simCalcUpdate;
	bool cancelSimulation = false;
	bool pauzeSimulation = false;

	std::mutex currentGenerationLock;

	std::vector<std::thread> simUpdaters;
	std::condition_variable nextUpdateCv;
	std::map<int, ThreadCombo*> threadComboData;
	unsigned int totalThreads;

	std::mutex lastPartLock;
	std::condition_variable lastPartGenerationCv;
	unsigned long lastPartGeneration;
	// The thread that processes the last parts of the cells list
	std::thread lastPartProcessor;
	// Simulation speed in Hz
	float targetSimulationSpeed = 0.0f;

	// Lock for when you need to edit the cells
	std::shared_mutex cellsEditLock;

	typedef std::map<std::pair<long, long>, Cell*>::size_type mapSizeType;
public:
	std::map<std::pair<long, long>, Cell*> cells;

	unsigned long currentGeneration = 0;
	float lastUpdateDuration = 0;


	std::string description;
	std::string author;

private:
	void LoadFile();
	void EmptyWorld();
	void IncrementNeighbors(long a_x, long a_y);
	void ProcessPartContinuesly(unsigned int  a_threadId, unsigned int a_maxThreads);
	void ProcessLastPart();
	void TimerThread();
	void World::InitializeThreads();
public:
	World();
	// Copy constructor
	World(const World& a_that);
	// Copy assignment operator
	World& operator=(const World& a_that);
	
	~World();

	void Save(std::string a_worldName);
	void Open(std::string a_filePath);
	void SaveAsTemplate(std::string a_worldName);

	void UpdateSimulationWithSingleGeneration();
	void StartSimulation();
	void PauzeSimulation();
	void ResetSimulation();

	void SetTargetSpeed(float a_targetSpeed);
	float GetTargetSpeed() { return this->targetSimulationSpeed; };

	Cell* GetCopyOfCellAt(long a_cellX, long a_cellY);
	bool UpdateCellState(long a_cellX, long a_cellY, std::function<bool (Cell*)> a_updater);
	bool InsertCellAt(long a_cellX, long a_cellY, CellState a_state);
	void LoadBlockAt(PremadeBlock a_premadeBlock, long a_cellX, long a_cellY);
};

#endif // !__WORLD__
