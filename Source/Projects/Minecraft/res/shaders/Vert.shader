#version 450

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inTexCoords;

layout(location = 0) out vec3 fragPos;
layout(location = 1) out vec3 fragTexCoords;

layout(binding = 0, set = 0) uniform uWorld
{
	mat4 proj;
	mat4 view;
} World;

void main()
{
	fragPos = inPos;
	fragTexCoords = inTexCoords;

	gl_Position = World.proj * World.view * vec4(inPos, 1.0);
}