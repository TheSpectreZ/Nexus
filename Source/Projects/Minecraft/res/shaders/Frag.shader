#version 450

layout(location = 1) in vec3 fragColor;
layout(location = 2) in vec3 fragNorm;
layout(location = 3) in vec2 fragTexC;

layout(binding = 1, set = 0) uniform uInstance
{
	mat4 transform;
	vec4 albedo;
} Instance;

layout(binding = 2, set = 0) uniform sampler2D uTexture;

layout(location = 0) out vec4 OutColor;

void main()
{
	OutColor = texture(uTexture, fragTexC);
}