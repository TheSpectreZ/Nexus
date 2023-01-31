#version 450

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNorm;
layout(location = 3) in vec2 inTexC;

layout(location = 1) out vec3 fragColor;
layout(location = 2) out vec3 fragNorm;
layout(location = 3) out vec2 fragTexC;

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
	fragNorm = inNorm;
	fragTexC = inTexC;

	gl_Position = World.proj * World.view * Instance.transform * vec4(inPos, 1.0);
}