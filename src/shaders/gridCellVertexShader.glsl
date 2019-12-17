#version 330 core
layout (location = 0) in vec2 aPos;

uniform mat4 u_Projection;

uniform int u_VerticalOffset = 0;
uniform int u_HorizontalOffset = 0;
uniform float u_CellWidth = 1;
uniform float u_CellHeight = 1;

void main()
{
	// Render cells
	gl_Position = u_Projection * vec4(aPos.x + (u_CellWidth * u_HorizontalOffset), aPos.y + (u_CellHeight * u_VerticalOffset), 1.0, 1.0);
}