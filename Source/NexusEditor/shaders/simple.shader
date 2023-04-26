#shader VERTEX
#version 450 core

layout(location = 0) in vec3 InPosition;
layout(location = 1) in vec3 InNormal;

layout(location = 0) out vec3 FragNormal;

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
	FragNormal = vec3(m_InstanceBuffer.Transform * vec4(InNormal,1.0));

	gl_Position = m_SceneBuffer.Projection * m_SceneBuffer.View * m_InstanceBuffer.Transform * vec4(InPosition, 1.0);
}

#shader FRAGMENT
#version 450 core

layout(location = 0) in vec3 FragNormal;
layout(location = 0) out vec4 OutColor;

void main()
{
	OutColor = vec4(FragNormal, 1.0);
}