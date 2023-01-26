#version 450

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inColor;

layout(location = 1) out vec3 fragColor;

void main()
{
	fragColor = inColor;

	gl_Position = vec4(inPos, 1.0);
}