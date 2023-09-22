#shader VERTEX
#version 450 core

layout(location = 0) out vec2 FragTexC;

void main() 
{
	FragTexC = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);
	gl_Position = vec4(FragTexC * 2.0f - 1.0f, 0.0f, 1.0f);
}

#shader FRAGMENT
#version 450 core

layout(location = 0) in vec2 FragTexC;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform sampler2D SamplerPosition;
layout(set = 0, binding = 1) uniform sampler2D SamplerNormal;
layout(set = 0, binding = 2) uniform sampler2D SamplerBaseColor;

void main()
{
	outColor = texture(SamplerBaseColor, FragTexC);
}