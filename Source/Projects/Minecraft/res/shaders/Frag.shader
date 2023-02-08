#version 450

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragTexCoords;

layout(location = 0) out vec4 outColor;

const float fogStart = 50.0;
const float fogEnd = 150.0;
const vec4 FogColor = vec4(0.9, 0.9, 0.9, 1.0);

layout(binding = 1, set = 0) uniform uInstance
{
	vec3 cameraPosition;
	float null;
} Instance;

layout(binding = 2, set = 0) uniform sampler2D utexture;

void main()
{
	float FogFactor = (fogEnd - length(Instance.cameraPosition - fragPos)) / (fogEnd - fogStart);

	outColor = texture(utexture, fragTexCoords.xy) * fragTexCoords.z;
//	outColor.a = 1.0;
//	outColor = (FogFactor * outColor) + (1 - FogFactor) * FogColor;
}