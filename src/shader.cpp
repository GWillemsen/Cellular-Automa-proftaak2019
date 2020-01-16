#include <iostream>
#include <fstream>
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "shader.h"

// Sets the vertex shader of this program.
bool Shader::SetVertexShader(std::string a_pathToShaderSourceFile)
{
    std::string shaderSource = this->LoadShaderSource(a_pathToShaderSourceFile);
    this->vertexShaderSource = shaderSource.c_str();

    // Create the new vertex shader.
    this->vertexShaderID = glCreateShader(GL_VERTEX_SHADER);

    // Add the shader code to the shader.
    glShaderSource(this->vertexShaderID, 1, &this->vertexShaderSource, NULL);
    glCompileShader(this->vertexShaderID);

    if(!this->ShaderCompiled(this->vertexShaderID))
        return false;

    return true;
}

bool Shader::SetFragmentShader(std::string a_pathToShaderSourceFile)
{
    std::string shaderSource = this->LoadShaderSource(a_pathToShaderSourceFile);
    this->fragmentShaderSource = shaderSource.c_str();

    // Create the new vertex shader.
    this->fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    // Add the shader code to the shader.
    glShaderSource(this->fragmentShaderID, 1, &this->fragmentShaderSource, NULL);
    glCompileShader(this->fragmentShaderID);

    if(!this->ShaderCompiled(this->fragmentShaderID))
        return false;

    return true;
}

int Shader::Compile()
{
    this->shaderProgram = glCreateProgram();

    // Attach the vertex and fragment shader.
    glAttachShader(this->shaderProgram, this->vertexShaderID);
    glAttachShader(this->shaderProgram, this->fragmentShaderID);
    
    // Compile the shader.
    glLinkProgram(this->shaderProgram);

    int compiled;
    char infoLog[512];

    // Get the Compile status.
    glGetProgramiv(this->shaderProgram, GL_LINK_STATUS, &compiled);

    if(!compiled)
    {
        glGetShaderInfoLog(this->shaderProgram, 512, NULL, infoLog);
        std::cerr << "ERROR: Could not compile shader program: " << infoLog << std::endl; 
    
        return -1;
    }

    // Delete the shaders.
    glDeleteShader(this->vertexShaderID);
    glDeleteShader(this->fragmentShaderID);

    return this->shaderProgram;
}

GLint Shader::GetUniformLocation(std::string a_uniformName)
{
	GLint m_location = glGetUniformLocation(this->shaderProgram, a_uniformName.c_str());

    return m_location;
}

void Shader::Use()
{
    glUseProgram(this->shaderProgram);
}

std::string Shader::LoadShaderSource(std::string a_pathToShaderSourceFile)
{
    std::ifstream fileReader;
    fileReader.open(a_pathToShaderSourceFile, std::ios::binary);

    // Make sure to check that the file is open.
    if(fileReader.is_open())
    {
        // Set the 'cursor' at the beginning of the file.
        fileReader.seekg(0, std::ios::beg);

        std::string line;
        std::string output;
        while (std::getline(fileReader, line))
        {
            output.append(line);
            output.append("\n");
        }

        // Add a null terminator at the end of the string.
        output.append("\0");

        return output;
    }
    else
    {
        std::cerr << "Could not open file." << std::endl;
    }

    return "";
}

bool Shader::ShaderCompiled(unsigned int a_id)
{
    int compiled;
    char infoLog[512];

    // Get the Compile status.
    glGetShaderiv(a_id, GL_COMPILE_STATUS, &compiled);

    if(!compiled)
    {
        glGetShaderInfoLog(a_id, 512, NULL, infoLog);
        std::cerr << "ERROR: Could not compile shader: " << infoLog << std::endl; 
    
        return false;
    }

    return true;
}

unsigned int Shader::GetProgramId()
{
	if (!this->compiledShaderProgram)
		return this->Compile();
	else
		return this->shaderProgram;
}

void Shader::SetMatrixValue(std::string a_uniformname, const GLfloat* a_data)
{
    this->Use();
    glUniformMatrix4fv(this->GetUniformLocation(a_uniformname), 1, GL_FALSE, a_data);
}
