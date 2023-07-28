#shader VERTEX
#version 450 core

layout(location = 0) in vec3 InPos;

layout(set = 0, binding = 0) uniform CameraBuffer
{
	mat4 projection;
	mat4 view;
	vec3 position;
	float null;
} u_CameraBuffer;

void main()
{
    gl_Position = u_CameraBuffer.projection * u_CameraBuffer.view * vec4(InPos,1.0);
}

#shader FRAGMENT
#version 450 core

layout(location = 0) out vec4 OutColor;

void main()
{
	OutColor = vec4(0.0, 1.0, 0.0, 1.0);
}
