/*
MIT License

Copyright (c) 2020 Guylian Gilsing & Giel Willemsen

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/
#include <iostream>
#include <map>
#include <functional> // create your own lambda
#include <vector>
#include <condition_variable>
#include <shared_mutex>

#include "cell.h"
#include "config.h"
#include "coordinateType.h"

#ifndef __WORLD__
#define __WORLD__

class World
{

private:
	typedef unsigned long long generationType;
	typedef std::map<std::pair<coordinatePart, coordinatePart>, Cell*>::size_type mapSizeType;
	class ThreadCombo {
		public:
			std::mutex lock;
			std::condition_variable cv;
			generationType current_generation;
			ThreadCombo()
			{
				this->current_generation = 0;
			};
	};

	std::thread timerThread;
	std::mutex simCalcUpdateLock;
	std::condition_variable simCalcUpdate;
	bool cancelSimulation = false;
	bool pauzeSimulation = false;

	std::mutex currentGenerationLock;

	std::vector<std::thread> simUpdaters;
	std::condition_variable nextUpdateCv;
	std::map<unsigned int, ThreadCombo*> threadComboData;
	unsigned int totalThreads;

	std::mutex lastPartLock;
	std::condition_variable lastPartGenerationCv;
	generationType lastPartGeneration;
	// The thread that processes the last parts of the cells list
	std::thread lastPartProcessor;
	// Simulation speed in Hz
	float targetSimulationSpeed = 0.0f;

	// Lock for when you need to edit the cells
	std::shared_mutex cellsEditLock;

	cellCountType cellStatistics[3] = { 0,0,0 };
	generationType currentGeneration = 0;
	generationType loadedWorldGenerationOffset = 0;
	
	float deltaTime[30];
	unsigned int deltaTimeIndex = 0;
	float totalTime = 0;
public:
	std::map<std::pair<coordinatePart, coordinatePart>, Cell*> cells;

	float lastUpdateDuration = 0;

	std::string filePath;
	std::string description;
	std::string author;
	std::string name;

private:
	void LoadFile();
	void EmptyWorld();
	void IncrementNeighbors(coordinatePart a_x, coordinatePart a_y);
	void ProcessPartContinuesly(unsigned int a_threadId, unsigned int a_maxThreads);
	void ProcessLastPart();
	void TimerThread();
	void World::InitializeThreads();
	coordinatePart ParseCoordinatePartFromString(char* a_input, std::string::size_type a_from);
public:
	World();
	// Copy constructor
	World(const World& a_that);
	// Copy assignment operator
	World& operator=(const World& a_that);
	
	~World();

	void Save();
	void Open(std::string a_filePath);
	
	void UpdateSimulationWithSingleGeneration();
	void StartSimulation();
	void PauzeSimulation();
	void ResetSimulation();

	void SetTargetSpeed(float a_targetSpeed);
	float GetTargetSpeed() { return this->targetSimulationSpeed; };

	Cell* GetCopyOfCellAt(coordinatePart a_cellX, coordinatePart a_cellY);
	bool TryUpdateCell(coordinatePart a_cellX, coordinatePart a_cellY, std::function<bool (Cell*)> a_updater);
	bool TryInsertCellAt(coordinatePart a_cellX, coordinatePart a_cellY, CellState a_state);
	void InViewport(std::vector<Cell*>* a_output, coordinatePart a_x, coordinatePart a_y, unsigned int a_width, unsigned int a_height);
	bool TryDeleteCell(coordinatePart a_cellX, coordinatePart a_cellY);

	bool GetIsRunning() { return !this->pauzeSimulation; };
	std::array<cellCountType, 3> GetStatistics();
	std::pair<coordinatePart, coordinatePart> GetCenterCoordinates();
	void ResetToConductors();
	generationType GetDisplayGeneration();
};

#endif // !__WORLD__
