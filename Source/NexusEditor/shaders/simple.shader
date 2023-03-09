#shader VERTEX
#version 450 core

layout(location = 0) in vec3 InPos;
layout(location = 1) in vec3 InCol;

vec2 positions[3] = vec2[](
	vec2(0.0, -0.5),
	vec2(0.5, 0.5),
	vec2(-0.5, 0.5)
);

void main()
{
	gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
}

#shader FRAGMENT
#version 450 core


layout(location = 0) out vec4 OutColor;

void main()
{
	OutColor = vec4(0.2, 0.2, 0.5, 1.0);
}