#version 330 core
layout (location = 0) in vec2 aPos;

uniform mat4 u_ModelMatrix;
uniform mat4 u_ProjectionMatrix;

uniform int u_VerticalOffset = 0;
uniform int u_HorizontalOffset = 0;

void main()
{
	// Render cells
	gl_Position = u_ProjectionMatrix * u_ModelMatrix * vec4(aPos.x, aPos.y, 1.0, 1.0);
}