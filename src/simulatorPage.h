#include <iostream>

#include "page.h"
#include "cell.h"

#ifndef __SIMULATORPAGE__
#define __SIMULATORPAGE__

class SimulatorPage : public Page
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

#endif
