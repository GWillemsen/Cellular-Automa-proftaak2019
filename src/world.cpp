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
	// Updates the simulation

	// Write simulation logic here...
}

Cell *World::GetCellAt(long a_cellX, long a_cellY)
{
	// Retrieves the pointer of a cell at a specific grid coordinate
	return nullptr;
}

void World::LoadBlockAt(PremadeBlock a_premadeBlock, long a_cellX, long a_cellY)
{
	// Loads the content of a premade block at a specific grid coordinate
}