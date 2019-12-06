#version 330 core
layout (location = 0) in vec2 aPos;

uniform mat4 u_Projection;

uniform int u_ColCount;
uniform int u_ColOrRow;

void main()
{
	float offsetNumber = (1.0f / u_ColCount) * gl_InstanceID;
	
	if (u_ColOrRow == 1)
	{
		gl_Position = u_Projection * vec4(aPos.x + offsetNumber, aPos.y, 1.0, 1.0);
	}
	else
	{
		gl_Position = u_Projection * vec4(aPos.x, aPos.y + offsetNumber, 1.0, 1.0);
	}
}