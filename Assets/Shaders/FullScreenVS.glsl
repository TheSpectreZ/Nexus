#version 450 core

layout ( location = 0 ) out vec4 FragPos;
layout ( location = 1 ) out vec2 FragUV;

void main()
{
	FragUV = vec2( gl_VertexIndex & 1, gl_VertexIndex >> 1 );
	FragPos = vec4( (FragUV.x - 0.5)*2 , -(FragUV.y - 0.5)*2 , 0 , 1 );

	gl_Position = FragPos;
}