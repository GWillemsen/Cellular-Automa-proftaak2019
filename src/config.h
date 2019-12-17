#include <iostream>
#include <glm/glm.hpp>
#ifndef __CONFIG__
#define __CONFIG__

class Config
{
public:
	glm::vec4 backgroundColor = glm::vec4(0.017241379310344827f, 0.017241379310344827f, 0.017241379310344827f, 1.0f); // Color of the background
	glm::vec4 guideColor = glm::vec4(0.00392156862745098f, 0.005128205128205128f, 0.05555555555555555f, 1.0f); // Color of a conductor
	glm::vec4 headColor = glm::vec4(0.16666666666666666f, 0.012195121951219513f, 0.004524886877828055f, 1.0f); // Color of a head
	glm::vec4 tailColor = glm::vec4(0.004273504273504274f, 0.03125f, 0.02564102564102564f, 1.0f); // Color of a tail
	
	std::string templateFolder = "/templates";

private:
	Config() {}; // Constructor
};

#endif // !__CONFIG__
