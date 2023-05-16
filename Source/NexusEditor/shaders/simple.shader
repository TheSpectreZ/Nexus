#shader VERTEX
#version 450 core

layout(location = 0) in vec3 InPosition;
layout(location = 1) in vec3 InNormal;
layout(location = 2) in vec2 InTexCoord;

layout(location = 1) out vec3 FragNormal;
layout(location = 2) out vec2 FragTexCoord;

layout(set = 0, binding = 0) uniform SceneBuffer
{
	mat4 Projection;
	mat4 View;
} m_SceneBuffer;

layout(set = 1, binding = 0) uniform InstanceBuffer
{
	mat4 Transform;
} m_InstanceBuffer;

void main()
{
	FragNormal = InNormal;
	FragTexCoord = InTexCoord;

	gl_Position = m_SceneBuffer.Projection * m_SceneBuffer.View * m_InstanceBuffer.Transform * vec4(InPosition, 1.0);
}

#shader FRAGMENT
#version 450 core

layout(location = 1) in vec3 FragNormal;
layout(location = 2) in vec2 FragTexCoord0;
layout(location = 2) in vec2 FragTexCoord1;

layout(location = 0) out vec4 OutColor;

layout(set = 2, binding = 0) uniform MaterialBuffer
{
	vec4 AlbedoColor;
	float roughness;
	float metalness;

	vec2 nul;
	vec4 nul2;
	vec4 nul3;
} m_MaterialBuffer;

layout(set = 2, binding = 1) uniform sampler2D albedo;

const vec3 LightDir = vec3(1.0, 1.0, 1.0);

void main()
{
	vec3 albedoColor = texture(albedo, FragTexCoord).rgb * m_MaterialBuffer.AlbedoColor.rgb;

	vec3 norm = normalize(FragNormal);
	vec3 dir = normalize(LightDir);

	float diff = max(dot(norm, dir), 0.0);

	OutColor = vec4(albedoColor * diff, 1.0);
}