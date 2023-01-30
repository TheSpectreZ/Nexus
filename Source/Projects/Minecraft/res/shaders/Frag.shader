#version 450

layout(binding = 1, set = 0) uniform uInstance
{
	mat4 transform;
	vec4 albedo;
} Instance;

layout(location = 0) out vec4 OutColor;

void main()
{
	OutColor = Instance.albedo;
}