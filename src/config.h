#include <iostream>
#include <glm/glm.hpp>

static class Config
{
private:
	glm::vec4 backgroundColor(0.017241379310344827f, 0.017241379310344827f, 0.017241379310344827f); // Color of the background
	glm::vec4 guideColor(0.00392156862745098f, 0.005128205128205128f, 0.05555555555555555f); // Color of a conductor
	glm::vec4 headColor(0.16666666666666666f, 0.012195121951219513f, 0.004524886877828055f); // Color of a head
	glm::vec4 tailColor(0.004273504273504274f, 0.03125f, 0.02564102564102564f); // Color of a tail

public:
	std::string templateFolder = "/templates";
};