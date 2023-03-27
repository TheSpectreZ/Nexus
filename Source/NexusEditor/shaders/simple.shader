#shader VERTEX
#version 450 core

layout(location = 0) in vec3 InPosition;
layout(location = 1) in vec3 InNormal;

layout(location = 1) out vec3 FragNormal;

layout(set = 0, binding = 0) uniform InstanceBuffer
{
	mat4 Transform;
} m_InstanceBuffer;

layout(push_constant) uniform constants
{
	mat4 mat;
} PushConstants;

void main()
{
	FragNormal = InNormal;

	gl_Position = PushConstants.mat * vec4(InPosition, 1.0);
}

#shader FRAGMENT
#version 450 core

layout(location = 1) in vec3 FragNormal;

layout(location = 0) out vec4 OutColor;

layout(set = 0, binding = 1) uniform MaterialBuffer
{
	vec3 albedoColor;
} m_InstanceMaterial;

void main()
{
	OutColor = vec4(FragNormal, 1.0);
}