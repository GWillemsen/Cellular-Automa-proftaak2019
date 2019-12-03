#include <iostream>

#include "page.h"
#include "config.h"
#include "world.h"
#include "cell.h"

class SimulatorPage : public Page, public Config
{
private:
	Cell worldCells[];

private:
	void InitOpenGL();
	void InitImGui();
	void InitSimulator();
	void RenderOpenGL();
	void RenderImGui();
	void UpdateSimulation();
	void DisposeOpenGL();
	void DisposeImGui();
};