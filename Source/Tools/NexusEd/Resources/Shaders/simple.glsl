#shader VERTEX
#version 450 core

layout(location = 0) in vec3 InPos;
layout(location = 1) in vec3 InNorm;
layout(location = 2) in vec3 InTang;
layout(location = 3) in vec3 InBiTang;
layout(location = 4) in vec2 InTexC;

layout(location = 0) out vec3 OutPos;

void main()
{
	OutPos = InPos;
    gl_Position = vec4(InPos, 1.0);
}

#shader FRAGMENT
#version 450 core

layout(location = 0) in vec3 OutPos;
layout(location = 0) out vec4 OutColor;

void main()
{
	OutColor = vec4(OutPos, 1.0);
}
