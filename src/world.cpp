#include <thread>
#include <mutex>
#include <iterator>
#include <future>
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
	if (!a_multithreaded)
		return;
	// returns 0 when not able to detect, otherwise, the number of (logical) processors
	unsigned int maxThreads = std::thread::hardware_concurrency();
	if (maxThreads == 0)
		maxThreads = 1;
	maxThreads--;
	for (unsigned int m_threadId = 0; m_threadId < maxThreads; m_threadId++)
	{
		this->vec_simUpdaters.emplace_back(std::thread(&World::ProcessPartContinuesly, this, m_threadId, maxThreads));
		this->threadComboData.emplace(make_pair(m_threadId, new ThreadCombo()));
		//std::map<int, std::pair<std::mutex, std::pair<unsigned long, std::condition_variable>>>
		//const int val = 0;
		//this->threadAndCurrentGenSignaling2.insert(std::make_pair(m_mute, std::make_pair(10, 1)));
		//this->threadMutexes.insert(make_pair(0, std::make_pair(std::make_unique<std::mutex>(), std::make_pair((unsigned long)0, std::make_unique<std::condition_variable>()))));
		/*this->threadAndCurrentGenSignaling.insert(
			std::make_pair(
				m_threadId, 
				std::make_pair(
					m_mutex,
					std::make_pair(
						(unsigned long)0,
						m_cv
					)
				)
			)
		);*/
	}
	this->totalThreads = maxThreads;
	this->lastPartProcessor = std::thread(&World::ProcessLastPart, this);
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
}

void World::PauzeSimulation()
{
	// Pauzes the simulation
}

void World::ResetSimulation()
{
	// Resets the simulation
}


void World::UpdateSimulationContinuesly()
{	
	{
		std::lock_guard<std::mutex> m_lk(this->mt_generation);
		this->currentGeneration++;
		this->cv_nextUpdate.notify_all();
	}

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
	{
		if (m_wait)
		{
			std::unique_lock<std::mutex> m_locker(this->lastPartLock);
			this->lastPartGenerationCv.wait(m_locker, [this] {
				return this->lastPartGeneration >= this->currentGeneration;
			});
			m_locker.unlock();
		}
	}

	for (std::pair<std::pair<int, int>, Cell*> m_element : this->cells)
	{
		if ((m_element.second->atomic_neighborCount.load() == 1 || m_element.second->atomic_neighborCount.load() == 2) &&
			m_element.second->state != Background &&
			m_element.second->state != Tail)
		{
			m_element.second->decayState = Head;
		}
		m_element.second->state = m_element.second->decayState;
		m_element.second->atomic_neighborCount.store(0);
	}
}

void World::ProcessPartContinuesly(int a_threadId, int a_threadCount)
{
	unsigned long m_nextToGenerateGeneration = 1;
	std::unique_lock<std::mutex> m_lk(this->mt_generation);
	auto m_iterator = this->cells.begin();
	auto m_sectionEnd = this->cells.end();
	auto m_lastEndPosition = this->cells.size();
	long m_lastIteratorPos = 0;


	while (!this->cancelSimulation)
	{
		// lk is only locked when the predicate is checked and if it returns true. 
		// otherwise it will unlock lk allowing other threads to check the condition as well
		// the whole loop set, see -> https://stackoverflow.com/a/2763749

		this->cv_nextUpdate.wait(m_lk, [this, m_nextToGenerateGeneration] {return m_nextToGenerateGeneration <= this->currentGeneration; });
		// unlock lk. we only needed the lock to check the currentGeneration.
		m_lk.unlock();
		if (!this->cancelSimulation)
		{
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
	std::unique_lock<std::mutex> m_lk(this->mt_generation);
	auto m_iterator = this->cells.begin();
	auto m_sectionEnd = this->cells.end();
	auto m_lastCellCount = this->cells.size();
	long m_lastIteratorPos = 0;


	while (!this->cancelSimulation)
	{
		// lk is only locked when the predicate is checked and if it returns true. 
		// otherwise it will unlock lk allowing other threads to check the condition as well
		// the whole loop set, see -> https://stackoverflow.com/a/2763749

		this->cv_nextUpdate.wait(m_lk, [this, m_nextToGenerateGeneration] {return m_nextToGenerateGeneration <= this->currentGeneration; });
		// unlock lk. we only needed the lock to check the currentGeneration.
		m_lk.unlock();
		if (!this->cancelSimulation)
		{
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

void World::IncrementNeighbors(int a_x, int a_y)
{
	using std::make_pair;
	std::pair<int, int> next;
	auto m_end = this->cells.end();

	// run through the Moore neighbors
	for (char x_offset = -1; x_offset < 2; x_offset++)
	{
		for (char y_offset = -1; y_offset < 2; y_offset++)
		{
			if (!(y_offset == 0 && x_offset == 0))
			{
				next = make_pair(a_x + x_offset, a_y + y_offset);
				auto m_found = this->cells.find(next);
				if (m_found != m_end && m_found->second->state == Conductor)
				{
					m_found->second->atomic_neighborCount.fetch_add(1);
				}
			}
		}
	}
}

void World::UpdateSimulationSerial()
{
	using std::pair;
	using std::make_pair;
	using std::map;

	// Updates the simulation

	// Write simulation logic here...

	for (pair<pair<int, int>, Cell*> m_element : this->cells)
	{
		if (m_element.second->state == Tail)
		{
			m_element.second->decayState = Conductor;
		}
		else if (m_element.second->state == Head)
		{
			this->IncrementNeighborsOld(m_element.second->x, m_element.second->y);
			m_element.second->decayState = Tail;
		}
	}

	for (pair<pair<int, int>, Cell*> m_element : this->cells)
	{
		if ((m_element.second->neighborCount == 1 || m_element.second->neighborCount == 2) &&
			m_element.second->state != Background &&
			m_element.second->state != Tail)
		{
			m_element.second->decayState = Head;
		}
		m_element.second->state = m_element.second->decayState;
		m_element.second->neighborCount = 0;
	}
}

void World::IncrementNeighborsOld(int a_x, int a_y)
{
	using std::make_pair;
	std::pair<int, int> next;

	// run through the Moore 
	for (char x_offset = -1; x_offset < 2; x_offset++)
	{
		for (char y_offset = -1; y_offset < 2; y_offset++)
		{
			if (!(y_offset == 0 && x_offset == 0))
			{
				next = make_pair(a_x + x_offset, a_y + y_offset);
				auto found = this->cells.find(next);
				if (found != this->cells.end() && this->cells[next]->state == Conductor)
				{
					this->cells[next]->neighborCount++;
				}
			}
		}
	}
}

Cell* World::GetCellAt(long a_cellX, long a_cellY)
{
	// Retrieves the pointer of a cell at a specific grid coordinate
	return nullptr;
}

void World::LoadBlockAt(PremadeBlock a_premadeBlock, long a_cellX, long a_cellY)
{
	// Loads the content of a premade block at a specific grid coordinate
}
