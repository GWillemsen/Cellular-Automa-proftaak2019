#include "cell.h"

class PremadeBlock
{
public:
	Cell cells[];

public:
	void LoadFromFile(std::string a_filePath);
};