#version 330 core
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
}