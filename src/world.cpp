#include <thread>
#include <mutex>
#include <iterator>
#include <algorithm>

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

	// Write simulation logic here...

	for (pair<pair<int, int>, Cell*> m_element : this->cells)
	{
		if (m_element.second->state == Tail)
		{
			m_element.second->decayState = Conductor;
		}
		else if (m_element.second->state == Head)
		{
			this->IncrementNeighbors(m_element.second->x, m_element.second->y);
			m_element.second->decayState = Tail;
		}
	}

	for (pair<pair<int, int>, Cell*> m_element : this->cells)
	{
		if ((m_element.second->neighorCount == 1 || m_element.second->neighorCount == 2) && 
			m_element.second->state != Background &&
			m_element.second->state != Tail)
		{
			m_element.second->decayState = Head;
		}
		m_element.second->state = m_element.second->decayState;
		m_element.second->neighorCount = 0;
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
					this->cells[next]->neighorCount++;
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
