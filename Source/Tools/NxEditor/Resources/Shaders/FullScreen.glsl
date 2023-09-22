#shader VERTEX
#version 450 core

layout(location = 0) out vec2 FragTexC;

void main()
{
	FragTexC = vec2( gl_VertexIndex & 1, gl_VertexIndex >> 1 );
	gl_Position = vec4( (FragTexC.x - 0.5) * 2, -(FragTexC.y - 0.5) * 2, 0, 1);
}

#shader FRAGMENT
#version 450 core

layout(location = 0) in vec2 FragTexC;
layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform sampler2D InScreenTexture;

void main()
{
	outColor = texture(InScreenTexture, FragTexC);
}