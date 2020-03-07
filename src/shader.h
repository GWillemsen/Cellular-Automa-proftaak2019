/*
MIT License

Copyright (c) 2020 Guylian Gilsing & Giel Willemsen

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/
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
