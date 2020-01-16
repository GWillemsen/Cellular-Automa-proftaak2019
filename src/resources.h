#include <map>
#include <string>

#ifndef __resources__
#define __resources__


const std::string basicFragmentShaderGlsl = R"(#version 330 core
out vec4 FragColor;
uniform vec4 u_Color;
void main()
{
	FragColor = u_Color;
};)";

const std::string gridCellFragmentShaderGlsl = R"(#version 330 core
out vec4 FragColor;
in vec3 color;

void main()
{
   FragColor = vec4(color.x, color.y, color.z, 1.0f);
};)";

const std::string gridLineVertexShaderGlsl = R"(#version 330 core
layout(location = 0) in vec2 aPos;
uniform mat4 u_ProjectionMatrix;
uniform int u_CellSizeInPx;
uniform int u_DrawHorizontal;
void main()
{
	float offsetNumber = u_CellSizeInPx * gl_InstanceID;
	if (u_DrawHorizontal == 1)
	{
		gl_Position = u_ProjectionMatrix * vec4(aPos.x, aPos.y + offsetNumber, 1.0, 1.0);
	}
	else
	{
		gl_Position = u_ProjectionMatrix * vec4(aPos.x + offsetNumber, aPos.y, 1.0, 1.0);
	}
};)";

const std::string gridCellVertexShaderGlsl = R"(#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 2) in vec2 aInstanceOffset;
layout (location = 3) in vec3 aColor;

uniform mat4 u_ModelMatrix;
uniform mat4 u_ProjectionMatrix;
out vec3 color;

void main()
{
	// Render cells
	vec4 m_newPosition = u_ModelMatrix * vec4(aPos.x, aPos.y, 1.0, 1.0);
	float m_newX = m_newPosition.x + aInstanceOffset.x;
	float m_newY = m_newPosition.y + aInstanceOffset.y;
	gl_Position = u_ProjectionMatrix * vec4(m_newX, m_newY, m_newPosition.z, m_newPosition.w);
	color = aColor;
})";

const std::string imguiIni = R"([Window][Debug##Default]
Pos=0,0
Size=400,400
Collapsed=0

[Window][Debug]
Pos=0,330
Size=370,150
Collapsed=0

[Window][Brush]
Pos=0,160
Size=240,160
Collapsed=0

[Window][World options]
Pos=0,20
Size=370,130
Collapsed=0

[Window][Save world file##saveWorldFile]
Pos=60,60
Size=960,400
Collapsed=0

[Window][Choose world file##chooseWorldFile]
Pos=60,60
Size=960,400
Collapsed=0

)";


std::map<std::string, std::map<std::string, std::string>> GetResources()
{
	std::map<std::string, std::map<std::string, std::string>> m_resources;
	std::map<std::string, std::string> m_shaders;
	m_shaders.emplace(std::make_pair("basicFragmentShader.glsl", basicFragmentShaderGlsl));
	m_shaders.emplace(std::make_pair("gridLineVertexShader.glsl", gridLineVertexShaderGlsl));
	m_shaders.emplace(std::make_pair("gridCellFragmentShader.glsl", gridCellFragmentShaderGlsl));
	m_shaders.emplace(std::make_pair("gridCellVertexShader.glsl", gridCellVertexShaderGlsl));
	
	std::map<std::string, std::string> m_inRoot;
	m_inRoot.emplace(std::make_pair("imgui.ini", imguiIni));
	
	m_resources.emplace(std::make_pair("shaders", m_shaders));
	m_resources.emplace(std::make_pair("", m_inRoot));
	return m_resources;
}
#endif