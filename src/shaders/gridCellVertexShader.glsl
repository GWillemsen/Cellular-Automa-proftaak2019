#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 2) in vec2 aInstanceOffset;

uniform mat4 u_ModelMatrix;
uniform mat4 u_ProjectionMatrix;

void main()
{
	// Render cells
	vec4 m_newPosition = u_ProjectionMatrix  * u_ModelMatrix * vec4(aPos.x, aPos.y, 1.0, 1.0);
	m_newPosition.x += aInstanceOffset.x;
	m_newPosition.y += aInstanceOffset.y;
	gl_Position = m_newPosition;
}