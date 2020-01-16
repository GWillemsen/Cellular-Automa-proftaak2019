#version 330 core
layout (location = 0) in vec2 aPos;

uniform mat4 u_ProjectionMatrix;

// Grid line render uniforms
uniform int u_CellSizeInPx;
uniform int u_DrawHorizontal;

void main()
{
	// Calculate the offset for the cells and grid lines
	float m_offsetNumber = u_CellSizeInPx * gl_InstanceID;

	// Render grid lines
	if (u_DrawHorizontal == 1)
	{
		gl_Position = u_ProjectionMatrix * vec4(aPos.x, aPos.y + m_offsetNumber , 1.0, 1.0);
	}
	else
	{
		gl_Position = u_ProjectionMatrix * vec4(aPos.x + m_offsetNumber , aPos.y, 1.0, 1.0);
	}
}