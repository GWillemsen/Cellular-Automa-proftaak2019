#include <thread>
#include <mutex>
#include <iterator>
#include <future>

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

void World::UpdateSimulation()
{
	using std::pair;
	using std::make_pair;
	using std::map;
	// Updates the simulation
	unsigned maxThreads = std::thread::hardware_concurrency();
	if (maxThreads == 0 )
		maxThreads = 1;

	if (this->cells.size() < 50 || maxThreads == 1)
	{
		this->UpdateSimulationSerial();
	}
	else
	{
		// returns 0 when not able to detect, otherwise, the number of (logical) processors
		int m_perThread = this->cells.size() / (float)maxThreads;
		std::vector<std::future<void>> m_runners;
		for (int m_threadId = 0; m_threadId < maxThreads; m_threadId++)
		{
			int m_start = m_threadId * m_perThread;
			// Start a new task and put it on the list (you cannot copy it!)
			m_runners.push_back(std::async(std::launch::async, &World::ProcessPart, this, m_start, m_perThread));
		}
		int m_spentOnOthers = maxThreads * m_perThread;
		this->ProcessPart(m_spentOnOthers, this->cells.size() - m_spentOnOthers);
		
		std::vector<std::future<void>>::iterator m_runnerIterator = m_runners.begin();
		while (m_runnerIterator != m_runners.end())
		{
			// Force the task to be resulted by blocking the current thread until the task returns
			(*m_runnerIterator).get();
			m_runnerIterator++;
		}


		for (pair<pair<int, int>, Cell*> m_element : this->cells)
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
}

void World::ProcessPart(int a_start, int a_count)
{
	// get the iterator that is at the beginning of the world
	// get the element at the a_start position
	std::map<std::pair<int, int>, Cell*>::iterator m_beginIterator = this->cells.begin();
	std::advance(m_beginIterator, a_start);
	/*std::map<std::pair<int,int>, Cell*>::iterator m_beginIterator = std::next(m_fullIterator, a_start);
	std::map<std::pair<int, int>, Cell*>::iterator m_endIterator = std::next(this->cells.begin(), ((long)a_start + (long)a_count));*/
	// get the element that is at the end 
	std::map<std::pair<int, int>, Cell*>::iterator m_endIterator = this->cells.begin();
	std::advance(m_endIterator, (long)a_start + a_count);

	// keep moving m_beginIterator forward until it reaches the m_endIterator position
	while (m_beginIterator != m_endIterator)
	{
		// An iterator item can be accessed like an pointer, you have to dereference it first.
		Cell* m_cell = (*m_beginIterator).second;
		if (m_cell->state == Tail)
		{
			m_cell->decayState = Conductor;
		}
		else if (m_cell->state == Head)
		{
			this->IncrementNeighbors(m_cell->x, m_cell->y);
			m_cell->decayState = Tail;
		}
		std::advance(m_beginIterator, 1);
	}
}


void World::IncrementNeighbors(int a_x, int a_y)
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
					this->cells[next]->atomic_neighborCount.fetch_add(1);
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
