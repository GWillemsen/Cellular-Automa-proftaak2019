#include <map>
#include <string>

#ifndef __resources__
#define __resources__


const std::string basicFragmentShaderGlsl = R"(#version 330 core
out vec4 FragColor;
uniform vec4 u_Color;
void main()
{
	FragColor = u_Color;//vec4(1.0f, 0.5f, 0.2f, 1.0f);
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
layout(location = 0) in vec2 aPos;
uniform mat4 u_ModelMatrix;
uniform mat4 u_ProjectionMatrix;
void main()
{
	gl_Position = u_ProjectionMatrix * u_ModelMatrix * vec4(aPos.x, aPos.y, 1.0, 1.0);
})";

const std::string imguiIni = R"([Window][Debug##Default]
Pos=0,0
Size=400,400
Collapsed=0

[Window][Debug]
Pos=0,280
Size=370,140
Collapsed=0

[Window][Brush]
Pos=0,160
Size=120,110
Collapsed=0

[Window][World options]
Pos=0,20
Size=370,130
Collapsed=0


)";


std::map<std::string, std::map<std::string, std::string>> GetResources()
{
	std::map<std::string, std::map<std::string, std::string>> m_resources;
	std::map<std::string, std::string> m_shaders;
	m_shaders.emplace(std::make_pair("basicFragmentShader.glsl", basicFragmentShaderGlsl));
	m_shaders.emplace(std::make_pair("gridLineVertexShader.glsl", gridLineVertexShaderGlsl));
	m_shaders.emplace(std::make_pair("gridCellVertexShader.glsl", gridCellVertexShaderGlsl));
	
	std::map<std::string, std::string> m_inRoot;
	m_inRoot.emplace(std::make_pair("imgui.ini", imguiIni));
	
	m_resources.emplace(std::make_pair("shaders", m_shaders));
	m_resources.emplace(std::make_pair("", m_inRoot));
	return m_resources;
}
#endif