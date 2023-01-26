#version 450

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inColor;

layout(location = 1) out vec3 fragColor;

layout(binding = 0, set = 0) uniform Data
{
	mat4 proj;
	mat4 view;
} Transform;

void main()
{
	fragColor = inColor;

	gl_Position = Transform.proj * Transform.view * vec4(inPos, 1.0);
}