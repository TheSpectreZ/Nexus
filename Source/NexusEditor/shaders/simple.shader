#shader VERTEX
#version 450 core

layout(location = 0) in vec3 InPosition;
layout(location = 1) in vec3 InNormal;
layout(location = 2) in vec2 InTexCoord;

layout(location = 0) out vec3 FragNormal;
layout(location = 1) out vec2 FragTexCoord;

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

layout(location = 0) in vec3 FragNormal;
layout(location = 1) out vec2 FragTexCoord;

layout(location = 0) out vec4 OutColor;

layout(set = 0, binding = 1) uniform sampler2D albedo;

void main()
{
	vec3 color = texture(albedo, FragTexCoord).rgb;

	OutColor = vec4(color, 1.0);
}