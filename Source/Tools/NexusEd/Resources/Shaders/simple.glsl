#shader VERTEX
#version 450 core

layout(location = 0) in vec3 InPos;
layout(location = 1) in vec3 InNorm;
layout(location = 2) in vec3 InTang;
layout(location = 3) in vec3 InBiTang;
layout(location = 4) in vec2 InTexC;

layout(location = 0) out vec3 OutPos;

layout(set = 0,binding = 0) uniform CameraBuffer
{
	mat4 projection;
	mat4 view;
} u_Camera;

layout(set = 1,binding = 0) uniform TransformBuffer
{
	mat4 model;
} u_Transform;

void main()
{
	OutPos = InPos ;
    gl_Position = u_Camera.projection * u_Camera.view * u_Transform.model * vec4(InPos, 1.0);
}

#shader FRAGMENT
#version 450 core

layout(location = 0) in vec3 OutPos;
layout(location = 0) out vec4 OutColor;

void main()
{
	OutColor = vec4(OutPos, 1.0);
}
