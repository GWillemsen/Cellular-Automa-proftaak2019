#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#ifndef __SHADER__
#define __SHADER__

// Creates a new shader program.
class Shader
{
private:
    unsigned int shaderProgram;

    unsigned int vertexShaderID;
    const char* vertexShaderSource;

    unsigned int fragmentShaderID;
    const char* fragmentShaderSource;

	bool compiledShaderProgram;

public:
    // Sets the vertex shader of this program.
    bool setVertexShader(std::string a_vertexShaderSource);
    bool setFragmentShader(std::string a_fragmentShaderSource);
    int getUniformLocation(std::string a_uniformName);
    int compile();
    void use();
	unsigned int getProgramId();

private:
    std::string loadShaderSource(std::string a_pathToShaderSourceFile);
    bool shaderCompiled(unsigned int a_id);
};

#endif // !__SHADER__
