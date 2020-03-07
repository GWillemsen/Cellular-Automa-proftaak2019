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
#include <thread>
#include <mutex>
#include <iterator>
#include <chrono>
#include <fstream>
#include <string>
#include <array>

#include "world.h"
#include "cell.h"

// Private methods
void World::LoadFile()
{
	// Opens a world from a file
	std::ifstream m_in(this->filePath, std::ios::in);
	std::vector<std::string> m_data;
	std::string m_buffer;
	if (!m_in.is_open())
	{
		m_in.open(this->filePath);
		if (!m_in.is_open())
			return;
	}

	// Read the header
	std::getline(m_in, m_buffer); // Line breaker is implicitly defined using a function overload to '\n'
	this->PauzeSimulation();
	this->EmptyWorld();
	auto m_worldNamePart = m_buffer.find(",", 0);
	if (m_worldNamePart > 0)
	{
		this->name = m_buffer.substr(0, m_worldNamePart);
		m_worldNamePart++;
		auto m_authorPart = m_buffer.find(",", m_worldNamePart);
		this->author = m_buffer.substr(m_worldNamePart, m_authorPart - m_worldNamePart);
		m_authorPart++;
		auto m_descriptionPart = m_buffer.find(",", m_authorPart);
		this->description = m_buffer.substr(m_authorPart, m_descriptionPart - m_authorPart);
		m_descriptionPart++;
		std::string m_currentGenerationData = m_buffer.substr(m_descriptionPart, m_buffer.length() - m_descriptionPart);
		this->loadedWorldGenerationOffset = std::stoull(m_currentGenerationData);
	}
	m_buffer.clear();
	// Evaluates to true while it a success
	while (std::getline(m_in, m_buffer))
	{
		std::string::size_type position = 0;

		auto m_xPart = m_buffer.find(',', 0);
		std::string m_xPartData = m_buffer.substr(0, m_xPart);
		coordinatePart m_readX = ParseCoordinatePartFromString(&m_xPartData[0], 0);

		// Consume the ,
		m_xPart++;
		// Find the next y reading from the xPart position and further
		auto m_yPart = m_buffer.find(',', m_xPart);
		std::string m_yPartData = m_buffer.substr(m_xPart, m_yPart - m_xPart);
		coordinatePart m_readY = ParseCoordinatePartFromString(&m_yPartData[0], 0);

		// Consume the ,
		m_yPart++;
		std::string m_statePartData = m_buffer.substr(m_yPart, m_buffer.length() - m_yPart);
		int m_readState = std::stoi(&m_statePartData[0], 0);
		if (m_readState > CellState::Background)
			m_readState = CellState::Background;

		this->TryInsertCellAt(m_readX, m_readY, (CellState)m_readState);
		m_buffer.clear();
	}
	m_in.close();
}

void World::EmptyWorld()
{
	// Empties the contents of a world
	this->cellsEditLock.lock();
	this->cells.clear();
	this->cellStatistics[0] = 0;
	this->cellStatistics[1] = 0;
	this->cellStatistics[2] = 0;
	this->cellsEditLock.unlock();
}

// Public methods

World::World()
{
	this->totalThreads = 0;
	this->cancelSimulation = false;
	this->pauzeSimulation = true;
	this->currentGeneration = 0;
	this->lastPartGeneration = 0;
	this->lastUpdateDuration = 0;
	this->targetSimulationSpeed = 1.0f;
	this->name = "Hello world";
	this->author = "John Doe";
	this->description = "A description";
	this->filePath = "";
	for (int m_index = 0; m_index < sizeof(this->deltaTime) / sizeof(float); m_index++)
	{
		this->deltaTime[m_index] = 0;
	}
	
	this->InitializeThreads();
}

void World::InitializeThreads()
{
	using std::pair;
	using std::make_pair;
	// Returns 0 when not able to detect, otherwise, the number of (logical) processors
	this->totalThreads = std::thread::hardware_concurrency();
	// Failsafe if not working properly
	if (this->totalThreads == 0)
		this->totalThreads = 1;

	// If we only get 2 threads or less, use 1 thread for the simulator.
	// Otherwise 1 for rendering, 1 for system and the others for the simulator.
	if (this->totalThreads < 3)
		this->totalThreads = 1;
	else
		this->totalThreads -= 2;
	
	// Calculate the number of thread that are extra (those will be created in the for loop below).
	for (unsigned int m_threadId = 0; m_threadId < this->totalThreads - 1; m_threadId++)
	{
		this->simUpdaters.emplace_back(std::thread(&World::ProcessPartContinuesly, this, m_threadId, this->totalThreads));
		this->threadComboData.emplace(make_pair(m_threadId, new ThreadCombo()));
	}
	// Set the total thread count to the number that we will actually be using.
	this->lastPartProcessor = std::thread(&World::ProcessLastPart, this);

	// Start the timer
	this->timerThread = std::thread(&World::TimerThread, this);
}

// 1. copy constructor
World::World(const World& that) : author(that.author), filePath(that.filePath), description(that.description) 
{
	this->cancelSimulation = that.cancelSimulation;
	this->lastUpdateDuration = 0;

	this->pauzeSimulation = that.pauzeSimulation;
	InitializeThreads();
	for (auto m_cell : that.cells)
	{
		this->cells.insert(
			std::make_pair(
				std::make_pair(m_cell.first.first, m_cell.first.second),
				new Cell(m_cell.second->x, m_cell.second->y, m_cell.second->cellState)
			)
		);
	}
	this->currentGeneration = that.currentGeneration;
	this->targetSimulationSpeed = that.targetSimulationSpeed;
	this->lastPartGeneration = that.lastPartGeneration;
}

// 2. copy assignment operator
World& World::operator=(const World& that) 
{
	this->cancelSimulation = that.cancelSimulation;
	this->lastUpdateDuration = 0;
	
	this->author = that.author;
	this->filePath = that.filePath;
	this->description = that.description;

	this->pauzeSimulation = that.pauzeSimulation;
	InitializeThreads();
	for (auto m_cell : that.cells)
	{
		this->cells.insert(
			std::make_pair(
				std::make_pair(m_cell.first.first, m_cell.first.second),
				new Cell(m_cell.second->x, m_cell.second->y, m_cell.second->cellState)
			)
		);
	}
	this->currentGeneration = that.currentGeneration;
	this->targetSimulationSpeed = that.targetSimulationSpeed;
	this->lastPartGeneration = that.lastPartGeneration;
	return *this;
}

// 3. destructor
World::~World()
{
	this->PauzeSimulation();
	// Start canceling the simulator updater's
	{
		std::lock_guard<std::mutex> m_lk(this->simCalcUpdateLock);
		this->cancelSimulation = true;
	}
	this->simCalcUpdate.notify_all();
	this->nextUpdateCv.notify_all();

	// Wait on all threads to return
	auto m_begining = this->simUpdaters.begin();
	auto m_end = this->simUpdaters.end();
	while (m_begining != m_end)
	{
		if (m_begining->joinable())
			m_begining->join();
		std::advance(m_begining, 1);
	}
	this->simUpdaters.clear();
	this->timerThread.join();
	this->lastPartProcessor.join();

	// Remove all cell data
	for (auto m_cell : this->cells)
	{
		delete m_cell.second;
	}
	this->cells.clear();
}

void World::Save()
{
	// Saves the world to a file
	std::ofstream m_out;
	m_out.open(this->filePath, std::ios::out);
	if (!m_out.is_open())
		return;
	m_out.write(&this->name[0], this->name.length());
	m_out.write(",", 1);
	m_out.write(&this->author[0], this->author.length());
	m_out.write(",", 1);
	m_out.write(&this->description[0], this->description.length());
	m_out.write(",", 1);
	unsigned long long m_toWriteGeneration = this->GetDisplayGeneration();
	auto m_str = std::to_string(m_toWriteGeneration);
	m_out.write(m_str.c_str(), m_str.length());
	m_out.write("\n", 1);

	this->cellsEditLock.lock();
	auto m_start = this->cells.begin();
	auto m_end = this->cells.end();
	while (m_start != m_end)
	{
		if (m_start->second->cellState == Background)
			continue;
		std::string m_x = std::to_string(m_start->second->x);
		m_out.write(&(m_x[0]), m_x.length());
		m_out.write(",", 1);
		
		std::string m_y = std::to_string(m_start->second->y);
		m_out.write(&(m_y[0]), m_y.length());
		m_out.write(",", 1);
		
		std::string m_state = std::to_string((int)m_start->second->cellState);
		m_out.write(&(m_state[0]), m_state.length());
		m_out.write("\n", 1);

		std::advance(m_start, 1);
	}
	m_out.close();
	this->cellsEditLock.unlock();

}

void World::Open(std::string a_filePath)
{
	// Loads the contents of a world file
	this->filePath = a_filePath;
	this->LoadFile();
}

void World::StartSimulation()
{
	{
		std::lock_guard<std::mutex> m_lk(this->simCalcUpdateLock);
		this->pauzeSimulation = false;
	}
	this->lastPartGenerationCv.notify_all();
}

void World::PauzeSimulation()
{
	{
		std::lock_guard<std::mutex> m_lk(this->simCalcUpdateLock);
		this->pauzeSimulation = true;
	}
	this->lastPartGenerationCv.notify_all();
}

void World::ResetSimulation()
{
	// Resets the simulation
	if (!this->filePath.empty())
		this->LoadFile();
	else
		this->EmptyWorld();
}

void World::UpdateSimulationWithSingleGeneration()
{
	{
		std::lock_guard<std::mutex> m_lk(this->currentGenerationLock);
		this->currentGeneration++;
	}
	this->nextUpdateCv.notify_all();

	auto m_begining = this->threadComboData.begin();
	auto m_ending = this->threadComboData.end();
	
	// Wait for the processing threads to catch up to the current generation
	while (m_begining != m_ending)
	{
		ThreadCombo* m_threadData = m_begining->second;
		generationType m_lastGenerationValue = m_threadData->current_generation;
		if (m_lastGenerationValue != this->currentGeneration)
		{
			std::unique_lock<std::mutex> m_locker(m_threadData->lock);
			m_begining->second->cv.wait(m_locker, [this, m_threadData] {
				return m_threadData->current_generation >= this->currentGeneration;
			});
		}
		std::advance(m_begining, 1);
	}
	
	// Check that the last part thread also is finished
	// Only go into wait if the thread hasn't already caught up
	bool m_wait = false;
	{
		std::lock_guard<std::mutex> m_locker(this->lastPartLock);
		if (this->lastPartGeneration != this->currentGeneration)
			m_wait = true;
	}

	if (m_wait)
	{
		std::unique_lock<std::mutex> m_locker(this->lastPartLock);
		this->lastPartGenerationCv.wait(m_locker, [this] {
			return this->lastPartGeneration >= this->currentGeneration;
		});
		m_locker.unlock();
	}

	//TODO multi thread this too?
	// Process the calculated results
	this->cellsEditLock.lock();
	cellCountType m_newHeadCount = 0;
	cellCountType m_newTailCount = 0;
	cellCountType m_newConductorCount = 0;
	for (auto m_cellPair : this->cells)
	{
		unsigned char m_neighborCount = m_cellPair.second->atomic_neighborCount.load();
		if ((m_neighborCount == 1 || m_neighborCount == 2) &&
			 m_cellPair.second->cellState != Background &&
			 m_cellPair.second->cellState != Tail)
		{
			m_cellPair.second->decayState = Head;
			m_newHeadCount += 1;
		}

		m_newTailCount += (m_cellPair.second->cellState == Tail);
		m_newConductorCount += (m_cellPair.second->cellState == Conductor);
		m_cellPair.second->cellState = m_cellPair.second->decayState;
		m_cellPair.second->atomic_neighborCount.store(0);
	}
	this->cellStatistics[0] = m_newHeadCount;
	this->cellStatistics[1] = m_newTailCount;
	this->cellStatistics[2] = m_newConductorCount;
	this->cellsEditLock.unlock();
}

void World::ProcessPartContinuesly(unsigned int a_threadId, unsigned int a_threadCount)
{
	generationType m_nextToGenerateGeneration = 1;
	std::unique_lock<std::mutex> m_lk(this->currentGenerationLock);
	auto m_iterator = this->cells.begin();
	auto m_sectionEnd = this->cells.end();
	mapSizeType  m_lastCellCount = this->cells.size();

	while (!this->cancelSimulation)
	{
		// m_lk is only locked when the predicate is checked and if it returns true. 
		// otherwise it will unlock m_lk allowing other threads to check the condition as well

		this->nextUpdateCv.wait(m_lk, [this, m_nextToGenerateGeneration] {
			return m_nextToGenerateGeneration <= this->currentGeneration || this->cancelSimulation;
		});

		// unlock m_lk. we only needed the lock to check the currentGeneration.
		m_lk.unlock();
		if (!this->cancelSimulation)
		{
			// Lock editing to the map
			this->cellsEditLock.lock_shared();

			mapSizeType m_cellCount = this->cells.size();
			mapSizeType m_perThread = m_cellCount / a_threadCount;
			mapSizeType m_nextPosition = m_perThread * a_threadId;
			if (m_cellCount > a_threadCount)
			{
				// Update the ending of the end processor
				if (m_cellCount != m_lastCellCount)
				{
					m_iterator = this->cells.begin();
					m_lastCellCount = m_cellCount;

					m_sectionEnd = this->cells.begin();
					std::advance(m_sectionEnd, m_nextPosition + m_perThread);
				}

				// Calculate what number to move (negative or positive) to get to the m_nextPosition value
				// Needs to be a signed number as m_toMove can go into the negative (for going backwards)
				long long m_difference = std::distance(this->cells.begin(), m_iterator);
				long long m_toMove = m_nextPosition - m_difference;
				std::advance(m_iterator, m_toMove);

				while (m_iterator != m_sectionEnd)
				{
					// An iterator item can be accessed like an pointer, you have to dereference it first.
					Cell* m_cell = (*m_iterator).second;
					if (m_cell->cellState == Tail)
					{
						m_cell->decayState = Conductor;
					}
					else if (m_cell->cellState == Head)
					{
						this->IncrementNeighbors(m_cell->x, m_cell->y);
						m_cell->decayState = Tail;
					}
					std::advance(m_iterator, 1);
				}
			}
			// Done with editing the map, unlock it again
			this->cellsEditLock.unlock_shared();
			// Notify main
			auto m_threadCombo = this->threadComboData.find(a_threadId);
			ThreadCombo* m_threadData = m_threadCombo->second;
			m_threadData->lock.lock();
			m_threadData->current_generation++;
			m_threadData->lock.unlock();
			m_threadData->cv.notify_all();
			m_nextToGenerateGeneration++;
		}
		// lock the m_lk again.
		m_lk.lock();
	}
}

void World::ProcessLastPart()
{
	generationType m_nextToGenerateGeneration = 1;
	std::unique_lock<std::mutex> m_lk(this->currentGenerationLock);
	auto m_iterator = this->cells.begin();
	auto m_sectionEnd = this->cells.end();
	mapSizeType  m_lastCellCount = this->cells.size();
	long m_lastIteratorPos = 0;

	while (!this->cancelSimulation)
	{
		// m_lk is only locked when the predicate is checked and if it returns true. 
		// otherwise it will unlock m_lk allowing other threads to check the condition as well

		this->nextUpdateCv.wait(m_lk, [this, m_nextToGenerateGeneration] {return m_nextToGenerateGeneration <= this->currentGeneration || this->cancelSimulation; });
		// unlock m_lk. we only needed the lock to check the currentGeneration.
		m_lk.unlock();
		if (!this->cancelSimulation)
		{
			// Lock editing to the map
			this->cellsEditLock.lock_shared();
			
			mapSizeType m_cellCount = this->cells.size();
			mapSizeType m_perThread = m_cellCount / this->totalThreads;
			
 			if (m_cellCount <= this->totalThreads)
				m_perThread = 0;

			mapSizeType m_nextPosition = m_perThread * (this->totalThreads - 1);
			// Update the ending of the end processor
			if (m_cellCount != m_lastCellCount)
			{
				m_sectionEnd = this->cells.end();
				m_iterator = this->cells.begin();
				m_lastCellCount = m_cellCount;
			}
			
			// Calculate what number to move (negative or positive) to get to the m_nextPosition value
			// Needs to be a signed number as m_toMove can go into the negative (for going backwards)
			long long m_difference = std::distance(this->cells.begin(), m_iterator);
			long long m_toMove = m_nextPosition - m_difference;
			std::advance(m_iterator, m_toMove);
			
			while (m_iterator != m_sectionEnd)
			{
				// An iterator item can be accessed like an pointer, you have to dereference it first.
				Cell* m_cell = (*m_iterator).second;
				if (m_cell->cellState == Tail)
				{
					m_cell->decayState = Conductor;
				}
				else if (m_cell->cellState == Head)
				{
					this->IncrementNeighbors(m_cell->x, m_cell->y);
					m_cell->decayState = Tail;
				}
				std::advance(m_iterator, 1);
			}

			// Done with map, unlock it
			this->cellsEditLock.unlock_shared();

			// Notify main
			this->lastPartLock.lock();
			this->lastPartGeneration++;
			this->lastPartLock.unlock();
			this->lastPartGenerationCv.notify_all();
			m_nextToGenerateGeneration++;
		}
		// lock the m_lk again.
		m_lk.lock();
	}
}

void World::IncrementNeighbors(coordinatePart a_x, coordinatePart a_y)
{
	std::pair<coordinatePart, coordinatePart> m_toFindPair;
	auto m_end = this->cells.end();

	// run through the Moore neighbors
	for (char m_xOffset = -1; m_xOffset < 2; m_xOffset++)
	{
		for (char m_yOffset = -1; m_yOffset < 2; m_yOffset++)
		{
			if (!(m_yOffset == 0 && m_xOffset == 0))
			{
				m_toFindPair = std::make_pair(a_x + m_xOffset, a_y + m_yOffset);
				auto m_found = this->cells.find(m_toFindPair);

				// Only if we found the cell, and it has the state of Conductor. increment the neighbor count
				if (m_found != m_end && m_found->second->cellState == Conductor)
				{
					m_found->second->atomic_neighborCount.fetch_add(1);
				}
			}
		}
	}
}

void World::SetTargetSpeed(float a_targetSpeed)
{
	{
		std::lock_guard<std::mutex> m_lk(this->simCalcUpdateLock);
		this->targetSimulationSpeed = a_targetSpeed;
	}
	this->simCalcUpdate.notify_all();
}

void World::TimerThread()
{
	const float m_defaultDurationOfOneFrameInMs = 1000.0;
	bool m_stop = false;
	std::chrono::time_point<std::chrono::high_resolution_clock> m_lastUpdatePoint = std::chrono::high_resolution_clock::now();
	bool m_fromTimeChangeEvent = false;
	while (!m_stop)
	{
		// Calculate the new time to wait for the next update
		this->simCalcUpdateLock.lock();
		float m_targetSpeed = this->targetSimulationSpeed;
		long m_interval = (long)(m_defaultDurationOfOneFrameInMs / m_targetSpeed);
		auto m_nowTime = std::chrono::high_resolution_clock::now();
		bool m_pauzed = this->pauzeSimulation;
		this->simCalcUpdateLock.unlock();

		// Check if we should update the simulation (based on the paused state)
		if (m_pauzed)
		{
			m_interval = 1000; // just force it to a interval of 1s
		}
		else
		{
			bool m_allowUpdate = true;
			if (m_fromTimeChangeEvent)
			{
				if (m_lastUpdatePoint + std::chrono::milliseconds(m_interval) > std::chrono::steady_clock::now())
				{
					m_allowUpdate = false;
				}
			}
			if (m_allowUpdate)
			{
				std::chrono::time_point<std::chrono::high_resolution_clock> m_starTime = std::chrono::high_resolution_clock::now();
				this->UpdateSimulationWithSingleGeneration();
				std::chrono::time_point<std::chrono::high_resolution_clock> m_endTime = std::chrono::high_resolution_clock::now();

				// Subtract the 2 time points from each other and calculate the difference between them in
				// milliseconds with floating point math.
				auto m_duration = std::chrono::duration<float, std::milli>(m_endTime - m_starTime);

				// Get the number of milliseconds
				auto m_durationInMs = m_duration.count();
				this->totalTime -= this->deltaTime[this->deltaTimeIndex];
				this->totalTime += m_durationInMs;
				this->deltaTime[this->deltaTimeIndex] = m_durationInMs;
				const int m_deltaTimeSize = sizeof(this->deltaTime) / sizeof(float);
				this->deltaTimeIndex++;
				this->deltaTimeIndex = ((this->deltaTimeIndex) % m_deltaTimeSize);
				this->lastUpdateDuration = this->totalTime / m_deltaTimeSize;
			}
		}

		// Keep looping until we are suppose to wake up. 
		// 1) That is either a the wait time has passed. 
		// 2) A new target speed has been set and we should start updating at the new speed.
		// 3) Or we are to cancel the simulation
		auto m_nextWake = m_nowTime + std::chrono::milliseconds(m_interval);
		bool m_nextRun = false;
		while (!m_nextRun)
		{
			std::unique_lock<std::mutex> m_lk(this->simCalcUpdateLock);
			
			// wait for either the simulation to be canceled or the speed to be changed
			// returns false if timeout has expired and _pred() is still false. otherwise it will return true
			bool m_waitResult = this->simCalcUpdate.wait_until(m_lk, m_nextWake, [this, m_targetSpeed] {
				return this->cancelSimulation || this->targetSimulationSpeed != m_targetSpeed;
			});
			m_lk.unlock();
			m_fromTimeChangeEvent = false;
			if (m_waitResult)
			{
				if (this->cancelSimulation)
				{
					m_stop = true;
					m_nextRun = true;
				}
				else
				{
					m_fromTimeChangeEvent = true;
					m_nextRun = true;
				}
			}
			else
			{
				m_nextRun = true;
			}
		}
	}
}

Cell* World::GetCopyOfCellAt(coordinatePart a_cellX, coordinatePart a_cellY)
{
	std::lock_guard<std::shared_mutex> m_lk(this->cellsEditLock);
	// Retrieves the pointer of a cell at a specific grid coordinate
	auto m_found = this->cells.find(std::make_pair(a_cellX, a_cellY));
	if (m_found == this->cells.end())
	{
		return nullptr;
	}
	else
	{
		return new Cell(m_found->second->x, m_found->second->y, m_found->second->cellState);
	}
}

bool World::TryInsertCellAt(coordinatePart a_cellX, coordinatePart a_cellY, CellState a_state)
{
	this->cellsEditLock.lock_shared();
	if (this->cells.find(std::make_pair(a_cellX, a_cellY)) != this->cells.end())
	{
		this->cellsEditLock.unlock_shared();
		return false;
	}
	this->cellsEditLock.unlock_shared();
	this->cellsEditLock.lock();
	this->cells.insert(std::make_pair(std::make_pair(a_cellX, a_cellY), new Cell(a_cellX, a_cellY, a_state)));
	if (a_state == Head)
		this->cellStatistics[0] += 1;
	else if (a_state== Tail)
		this->cellStatistics[1] += 1;
	else if (a_state == Conductor)
		this->cellStatistics[2] += 1;
	this->cellsEditLock.unlock();
	return true;
}

bool World::TryUpdateCell(coordinatePart a_cellX, coordinatePart a_cellY, std::function<bool (Cell*)> a_updater)
{
	this->cellsEditLock.lock_shared();
	auto m_found = this->cells.find(std::make_pair(a_cellX, a_cellY));
	if (m_found == this->cells.end())
	{
		this->cellsEditLock.unlock_shared();
		return false;
	}
	else
	{
		this->cellsEditLock.unlock_shared();
		this->cellsEditLock.lock();
		if (m_found->second->cellState == Head && this->cellStatistics[0] > 0)
			this->cellStatistics[0] -= 1;
		else if (m_found->second->cellState == Tail && this->cellStatistics[1] > 0)
			this->cellStatistics[1] -= 1;
		else if (m_found->second->cellState == Conductor && this->cellStatistics[2] > 0)
			this->cellStatistics[2] -= 1;
		
		bool m_result = a_updater(m_found->second);
		
		if (m_found->second->cellState == Head)
			this->cellStatistics[0] += 1;
		else if (m_found->second->cellState == Tail)
			this->cellStatistics[1] += 1;
		else if (m_found->second->cellState == Conductor)
			this->cellStatistics[2] += 1;

		this->cellsEditLock.unlock();
		return m_result;
	}
}

void World::InViewport(std::vector<Cell*>* a_output, coordinatePart a_x, coordinatePart a_y, unsigned int a_width, unsigned int a_height)
{
	long m_preCalcSize = (a_width * a_height) / 4;
	if (m_preCalcSize > 20)
		m_preCalcSize = 20;
	a_output->reserve(a_output->size() + m_preCalcSize);

	this->cellsEditLock.lock_shared();
	auto m_iterator = this->cells.begin();
	auto m_end = this->cells.end();
	coordinatePart m_endX = a_width + a_x;
	coordinatePart m_endY = a_height + a_y;
	while (m_iterator != m_end)
	{
		Cell* m_cell = m_iterator->second;
		if (m_cell->x > a_x&& m_cell->y > a_y)
		{
			if (m_cell->x < m_endX)
			{
				if (m_cell->y < m_endY)
				{
					a_output->push_back(m_cell);
				}
			}
		}
		std::advance(m_iterator, 1);
	}
	this->cellsEditLock.unlock_shared();
}

bool World::TryDeleteCell(coordinatePart a_cellX, coordinatePart a_cellY)
{
	this->cellsEditLock.lock_shared();
	auto m_found = this->cells.find(std::make_pair(a_cellX, a_cellY));
	if (m_found == this->cells.end())
	{
		this->cellsEditLock.unlock_shared();
		return false;
	}
	else
	{
		this->cellsEditLock.unlock_shared();
		this->cellsEditLock.lock();
		if (m_found->second->cellState == Head && this->cellStatistics[0] > 0)
			this->cellStatistics[0] -= 1;
		else if (m_found->second->cellState == Tail && this->cellStatistics[1] > 0)
			this->cellStatistics[1] -= 1;
		else if (m_found->second->cellState == Conductor && this->cellStatistics[2] > 0)
			this->cellStatistics[2] -= 1;
		this->cells.erase(m_found);
		this->cellsEditLock.unlock();
		return true;
	}
}

coordinatePart World::ParseCoordinatePartFromString(char* a_input, std::string::size_type a_from)
{
	return std::stoll(a_input, &a_from, 10);
}

std::array<cellCountType, 3> World::GetStatistics()
{
	return std::array<cellCountType, 3>{ this->cellStatistics[0], this->cellStatistics[1], this->cellStatistics[2] };
}

std::pair<coordinatePart, coordinatePart> World::GetCenterCoordinates()
{
	this->cellsEditLock.lock_shared();
	auto m_beginning = this->cells.cbegin();
	auto m_ending = this->cells.cend();
	coordinatePart m_minX = std::numeric_limits<coordinatePart>::max();
	coordinatePart m_minY = std::numeric_limits<coordinatePart>::max();
	coordinatePart m_maxX = std::numeric_limits<coordinatePart>::min();
	coordinatePart m_maxY = std::numeric_limits<coordinatePart>::min();
	
	while (m_beginning != m_ending)
	{
		if (m_beginning->second->y < m_minY)
			m_minY = m_beginning->second->y;
		if (m_beginning->second->x < m_minX)
			m_minX = m_beginning->second->x;

		if (m_beginning->second->y > m_maxY)
			m_maxY = m_beginning->second->y;
		if (m_beginning->second->x > m_maxX)
			m_maxX = m_beginning->second->x;
		std::advance(m_beginning, 1);
	}
	this->cellsEditLock.unlock_shared();
	coordinatePart m_centerX = m_minX + ((m_maxX - m_minX) / 2);
	coordinatePart m_centerY = m_minY + ((m_maxY - m_minY) / 2);

	return std::make_pair(m_centerX, m_centerY);
}

void World::ResetToConductors()
{
	this->cellsEditLock.lock();
	auto m_beginning = this->cells.begin();
	auto m_ending = this->cells.end();
	while (m_beginning != m_ending)
	{
		Cell* m_cell = m_beginning->second;
		if (m_cell->cellState != Conductor && m_cell->cellState != Background)
		{
			// Remove from stats
			if (m_cell->cellState == Head && this->cellStatistics[0] > 0)
				this->cellStatistics[0] -= 1;
			else if (m_cell->cellState == Tail && this->cellStatistics[1] > 0)
				this->cellStatistics[1] -= 1;

			// Actually change the cell state
			if (m_cell->cellState < Background)
				m_cell->cellState = Conductor;
			if (m_cell->decayState < Background)
				m_cell->decayState = Conductor;
			m_cell->atomic_neighborCount.store(0);


			// Count the new conductor
			this->cellStatistics[2] += 1;
		}
		std::advance(m_beginning, 1);
	}
	this->cellsEditLock.unlock();
}

unsigned long long World::GetDisplayGeneration()
{
	return this->currentGeneration + this->loadedWorldGenerationOffset;
}
