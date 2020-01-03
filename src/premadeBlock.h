#include "cell.h"

#ifndef __PREMADE_BLOCK__
#define __PREMADE_BLOCK__

class PremadeBlock
{
public:
	Cell cells[1];

public:
	void LoadFromFile(std::string a_filePath);
};

#endif // !__PREMADE_BLOCK__
