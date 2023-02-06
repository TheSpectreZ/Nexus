#version 450

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inColor;

layout(location = 1) out vec3 fragColor;

layout(binding = 0, set = 0) uniform uWorld
{
	mat4 proj;
	mat4 view;
} World;

layout(binding = 1, set = 0) uniform uInstance
{
	mat4 transform;
	vec4 albedo;
} Instance;

void main()
{
	fragColor = inColor;

	gl_Position = World.proj * World.view * Instance.transform * vec4(inPos, 1.0);
}