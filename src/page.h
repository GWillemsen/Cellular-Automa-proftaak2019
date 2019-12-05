#include <GLFW/glfw3.h>

#ifndef __PAGE__
#define __PAGE__

class Page
{
public:
	virtual Page* Run() = 0;
};

#endif // !__PAGE__
