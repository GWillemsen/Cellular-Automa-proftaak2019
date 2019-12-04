#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 u_Projection;
uniform int u_DrawDir;

void main()
{
	gl_Position = u_Projection * vec4(aPos.x + (gl_InstanceID * 32), aPos.y + (gl_InstanceID * 32), aPos.z, 1.0);
}