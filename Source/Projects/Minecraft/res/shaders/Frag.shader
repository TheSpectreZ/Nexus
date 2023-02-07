#version 450

layout(location = 1) in vec4 fragColor;

layout(location = 0) out vec4 OutColor;

void main()
{
	OutColor = vec4(fragColor.xyz * fragColor.w, 1.0);
}