#version 450

layout(location = 1) in vec3 fragColor;

layout(location = 0) out vec4 OutColor;

void main()
{
	OutColor = vec4(fragColor, 1.0);
}