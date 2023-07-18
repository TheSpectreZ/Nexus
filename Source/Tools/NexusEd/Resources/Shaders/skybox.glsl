#shader VERTEX
#version 450 core

layout(location = 0) in vec3 InPos;
layout(location = 0) out vec3 OutUV;

layout(set = 0, binding = 0) uniform CameraBuffer
{
	mat4 projection;
	mat4 view;
	vec3 position;
	float null;
} u_CameraBuffer;

void main()
{
	OutUV = InPos ;
	
	mat4 rotView = mat4(mat3( u_CameraBuffer.view));
    vec4 clipPos = u_CameraBuffer.projection * rotView * vec4(InPos, 1.0);

    gl_Position = clipPos.xyww;
}

#shader FRAGMENT
#version 450 core

layout(location = 0) in vec3 FragUV;
layout(location = 0) out vec4 OutColor;

layout(set = 0,binding = 1) uniform samplerCube envMap;

void main()
{
	OutColor =	texture(envMap,FragUV);
}
