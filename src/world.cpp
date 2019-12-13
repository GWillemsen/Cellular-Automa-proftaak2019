#include <thread>
#include <mutex>
#include <iterator>
#include <chrono>

#include "world.h"
#include "cell.h"

// Private methods
void World::LoadFile()
{
	// Loads the contents of a world file
}

void World::EmptyWorld()
{
	// Empties the contents of a world
}

// Public methods

World::World(bool a_multithreaded)
{
	using std::pair;
	using std::make_pair;
	this->totalThreads = 0;
	this->cancelSimulation = false;
	this->pauzeSimulation = true;
	this->currentGeneration = 0;
	this->lastPartGeneration = 0;
	this->lastUpdateDuration = 0;
	this->targetSimulationSpeed = 1.0f;
	if (!a_multithreaded)
		return;
	// returns 0 when not able to detect, otherwise, the number of (logical) processors
	this->totalThreads = std::thread::hardware_concurrency();
	if (this->totalThreads == 0)
		this->totalThreads = 1;
	this->totalThreads--;
	for (unsigned int m_threadId = 0; m_threadId < this->totalThreads; m_threadId++)
	{
		this->simUpdaters.emplace_back(std::thread(&World::ProcessPartContinuesly, this, m_threadId, this->totalThreads));
		this->threadComboData.emplace(make_pair(m_threadId, new ThreadCombo()));
	}
	this->totalThreads++;
	this->lastPartProcessor = std::thread(&World::ProcessLastPart, this);
	this->timerThread = std::thread(&World::TimerThread, this);
}

void World::Save(std::string a_worldName)
{
	// Saves the world to a file
}

void World::Open(std::string a_filePath)
{
	// Opens a world from a file
}

void World::SaveAsTemplate(std::string a_worldName)
{
	// Saves the world to a template file
}

void World::StartSimulation()
{
	// Starts the simulation
	std::lock_guard<std::mutex> m_lk(this->simCalcUpdateLock);
	this->pauzeSimulation = false;
	this->lastPartGenerationCv.notify_all();
}

void World::PauzeSimulation()
{
	// Pauses the simulation
	std::lock_guard<std::mutex> m_lk(this->simCalcUpdateLock);
	this->pauzeSimulation = true;
	this->lastPartGenerationCv.notify_all();
}

void World::ResetSimulation()
{
	// Resets the simulation
}

void World::UpdateSimulationWithSingleGeneration()
{
	// 
	{
		std::lock_guard<std::mutex> m_lk(this->currentGenerationLock);
		this->currentGeneration++;
	}
	this->nextUpdateCv.notify_all();

	auto m_begining = this->threadComboData.begin();
	auto m_ending = this->threadComboData.end();
	
	while (m_begining != m_ending)
	{
		ThreadCombo* m_threadData = m_begining->second;
		unsigned long m_lastGenerationValue = m_threadData->current_generation;
		if (m_lastGenerationValue != this->currentGeneration)
		{
			std::unique_lock<std::mutex> m_locker(m_threadData->lock);
			m_begining->second->cv.wait(m_locker, [this, m_threadData] {
				return m_threadData->current_generation >= this->currentGeneration;
			});
		}
		std::advance(m_begining, 1);
	}
	
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
	for (auto m_cellPair : this->cells)
	{
		if ((m_cellPair.second->atomic_neighborCount.load() == 1 ||
			 m_cellPair.second->atomic_neighborCount.load() == 2) &&
			m_cellPair.second->state != Background &&
			m_cellPair.second->state != Tail)
		{
			m_cellPair.second->decayState = Head;
		}
		m_cellPair.second->state = m_cellPair.second->decayState;
		m_cellPair.second->atomic_neighborCount.store(0);
	}
}

void World::ProcessPartContinuesly(unsigned int a_threadId, unsigned int a_threadCount)
{
	using std::pair;
	unsigned long m_nextToGenerateGeneration = 1;
	std::unique_lock<std::mutex> m_lk(this->currentGenerationLock);
	auto m_iterator = this->cells.begin();
	auto m_sectionEnd = this->cells.end();
	auto m_lastEndPosition = this->cells.size();
	long m_lastIteratorPos = 0;


	while (!this->cancelSimulation)
	{
		// lk is only locked when the predicate is checked and if it returns true. 
		// otherwise it will unlock lk allowing other threads to check the condition as well
		// the whole loop set, see -> https://stackoverflow.com/a/2763749

		this->nextUpdateCv.wait(m_lk, [this, m_nextToGenerateGeneration] {
			return m_nextToGenerateGeneration <= this->currentGeneration || this->cancelSimulation;
		});
		// unlock lk. we only needed the lock to check the currentGeneration.
		m_lk.unlock();
		if (!this->cancelSimulation)
		{
			this->cellsEditLock.lock_shared();
			long m_cellCount = this->cells.size();
			long m_perThread = m_cellCount / a_threadCount;
			long m_nextPosition = m_perThread * a_threadId;
			long m_difference = std::distance(this->cells.begin(), m_iterator);
			long m_toMove = m_nextPosition - m_difference;
			std::advance(m_iterator, m_toMove);

			if (m_nextPosition + m_perThread != m_lastEndPosition)
			{
				m_sectionEnd = this->cells.begin();
				std::advance(m_sectionEnd, m_nextPosition + m_perThread);
				m_lastEndPosition = m_nextPosition + m_perThread;
			}

			while (m_iterator != m_sectionEnd)
			{
				// An iterator item can be accessed like an pointer, you have to dereference it first.
				Cell* m_cell = (*m_iterator).second;
				if (m_cell->state == Tail)
				{
					m_cell->decayState = Conductor;
				}
				else if (m_cell->state == Head)
				{
					this->IncrementNeighbors(m_cell->x, m_cell->y);
					m_cell->decayState = Tail;
				}
				std::advance(m_iterator, 1);
			}
			this->cellsEditLock.unlock_shared();

			auto m_threadCombo = this->threadComboData.find(a_threadId);
			ThreadCombo* m_threadData = m_threadCombo->second;
			m_threadData->lock.lock();
			m_threadData->current_generation++;
			m_threadData->lock.unlock();
			m_threadData->cv.notify_all();
			m_nextToGenerateGeneration++;
		}
		// lock the lk again.
		m_lk.lock();
	}
}

void World::ProcessLastPart()
{
	unsigned long m_nextToGenerateGeneration = 1;
	std::unique_lock<std::mutex> m_lk(this->currentGenerationLock);
	auto m_iterator = this->cells.begin();
	auto m_sectionEnd = this->cells.end();
	auto m_lastCellCount = this->cells.size();
	long m_lastIteratorPos = 0;


	while (!this->cancelSimulation)
	{
		// lk is only locked when the predicate is checked and if it returns true. 
		// otherwise it will unlock lk allowing other threads to check the condition as well
		// the whole loop set, see -> https://stackoverflow.com/a/2763749

		this->nextUpdateCv.wait(m_lk, [this, m_nextToGenerateGeneration] {return m_nextToGenerateGeneration <= this->currentGeneration || this->cancelSimulation; });
		// unlock lk. we only needed the lock to check the currentGeneration.
		m_lk.unlock();
		if (!this->cancelSimulation)
		{
			this->cellsEditLock.lock_shared();
			long m_cellCount = this->cells.size();
			long m_perThread = m_cellCount / this->totalThreads;
			long m_nextPosition = m_perThread * this->totalThreads;
			long m_difference = std::distance(this->cells.begin(), m_iterator);
			long m_toMove = m_nextPosition - m_difference;
			std::advance(m_iterator, m_toMove);

			if (m_cellCount != m_lastCellCount)
			{
				m_sectionEnd = this->cells.end();
				m_lastCellCount = m_nextPosition + m_perThread;
			}

			while (m_iterator != m_sectionEnd)
			{
				// An iterator item can be accessed like an pointer, you have to dereference it first.
				Cell* m_cell = (*m_iterator).second;
				if (m_cell->state == Tail)
				{
					m_cell->decayState = Conductor;
				}
				else if (m_cell->state == Head)
				{
					this->IncrementNeighbors(m_cell->x, m_cell->y);
					m_cell->decayState = Tail;
				}
				std::advance(m_iterator, 1);
			}

			this->cellsEditLock.unlock_shared();
			this->lastPartLock.lock();
			this->lastPartGeneration++;
			this->lastPartLock.unlock();
			this->lastPartGenerationCv.notify_all();
			m_nextToGenerateGeneration++;
		}
		// lock the lk again.
		m_lk.lock();
	}
}

void World::IncrementNeighbors(long a_x, long a_y)
{
	std::pair<long, long> m_toFindPair;
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
				if (m_found != m_end && m_found->second->state == Conductor)
				{
					m_found->second->atomic_neighborCount.fetch_add(1);
				}
			}
		}
	}
}

void World::SetTargetSpeed(float a_targetSpeed)
{
	this->simCalcUpdateLock.lock();
	this->targetSimulationSpeed = a_targetSpeed;
	this->simCalcUpdateLock.unlock();
	this->simCalcUpdate.notify_all();
}

void World::TimerThread()
{
	bool m_stop = false;
	while (!m_stop)
	{
		this->simCalcUpdateLock.lock();
		float m_targetSpeed = this->targetSimulationSpeed;
		long m_int = (long)(1000.0 / (double)m_targetSpeed);
		auto m_nowTime = std::chrono::steady_clock::now();
		bool m_pauzed = this->pauzeSimulation;
		this->simCalcUpdateLock.unlock();

		if (m_pauzed)
		{
			m_int = 1000; // just force it to a interval of 1s
		}
		else 
		{
			clock_t m_starTime = clock();
			this->UpdateSimulationWithSingleGeneration();
			clock_t m_endTime = clock();
			float m_differenceInMs = (m_endTime - m_starTime) / (CLOCKS_PER_SEC / 1000.0f);
			this->lastUpdateDuration = m_differenceInMs;
		}
		
		auto m_nextWake = m_nowTime + std::chrono::milliseconds(m_int);
		bool m_nextRun = false;
		while (!m_nextRun)
		{
			std::unique_lock<std::mutex> m_lk(this->simCalcUpdateLock);
			
			// wait for either the simulation to be canceled or the speed to be changed
			// returns false if timeout has expired and pred() is still false. otherwise it will return true
			bool m_waitResult = this->simCalcUpdate.wait_until(m_lk, m_nextWake, [this, m_targetSpeed] {
				return this->cancelSimulation || this->targetSimulationSpeed != m_targetSpeed;
			});
			if (m_waitResult)
			{
				if (this->cancelSimulation)
				{
					m_stop = true;
					m_nextRun = true;
				}
				else
				{
					m_int = (long)(1000.0 * this->targetSimulationSpeed);
					auto m_newNextWake = m_nowTime + std::chrono::milliseconds(m_int);
					auto m_now = std::chrono::steady_clock::now() - std::chrono::milliseconds(250);
					if (m_newNextWake < m_now)
					{
						m_nextRun = true;
					}
					else
					{
						m_nextWake = m_newNextWake;
					}
				}
			}
			else
			{
				m_nextRun = true;
			}
			m_lk.unlock();
		}
	}
}

Cell* World::GetCopyOfCellAt(long a_cellX, long a_cellY)
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
		return new Cell(m_found->second->x, m_found->second->y, m_found->second->state);
	}
}

bool World::InsertCellAt(long a_cellX, long a_cellY, CellState a_state)
{
	std::lock_guard<std::shared_mutex> m_lk(this->cellsEditLock);
	if (this->cells.find(std::make_pair(a_cellX, a_cellY)) == this->cells.end())
		return false;
	this->cells.insert(std::make_pair(std::make_pair(a_cellX, a_cellY), new Cell(a_cellX, a_cellY, a_state)));
	return true;
}

bool World::UpdateCellState(long a_cellX, long a_cellY, std::function<bool (Cell*)> a_updater)
{
	std::lock_guard<std::shared_mutex> m_lk(this->cellsEditLock);
	auto m_found = this->cells.find(std::make_pair(a_cellX, a_cellY));
	if (m_found == this->cells.end())
	{
		return false;
	}
	else
	{
		return a_updater(m_found->second);
	}
}

void World::LoadBlockAt(PremadeBlock a_premadeBlock, long a_cellX, long a_cellY)
{
	// Loads the content of a premade block at a specific grid coordinate
}
