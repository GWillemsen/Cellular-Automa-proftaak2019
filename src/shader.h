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
    bool SetVertexShader(std::string a_vertexShaderSource);
    bool SetFragmentShader(std::string a_fragmentShaderSource);
	GLint GetUniformLocation(std::string a_uniformName);
    int Compile();
    void Use();
	unsigned int GetProgramId();
    void SetMatrixValue(std::string a_uniformname, const GLfloat* a_data);

private:
    std::string LoadShaderSource(std::string a_pathToShaderSourceFile);
    bool ShaderCompiled(unsigned int a_id);
};

#endif // !__SHADER__
