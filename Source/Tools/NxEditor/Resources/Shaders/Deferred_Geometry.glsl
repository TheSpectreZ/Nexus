#shader VERTEX
#version 450 core

layout(location = 0) in vec3 InPos;
layout(location = 1) in vec3 InNorm;
layout(location = 2) in vec3 InTang;
layout(location = 3) in vec3 InBiTang;
layout(location = 4) in vec2 InTexC;

layout(location = 0) out vec3 FragPos;
layout(location = 1) out vec3 FragNorm;
layout(location = 2) out vec2 FragTexC;
layout(location = 3) out mat3 FragTBN;

layout(set = 0, binding = 0) uniform CameraBuffer
{
	mat4 projection;
	mat4 view;
	vec3 position;
	float null;
} u_CameraBuffer;

layout(set = 1, binding = 0) uniform InstanceBuffer
{
	mat4 Transform;
} u_InstanceBuffer;

void main()
{
	vec3 T = normalize(vec3(u_InstanceBuffer.Transform * vec4(InTang, 0.0)));
	vec3 B = normalize(vec3(u_InstanceBuffer.Transform * vec4(InBiTang, 0.0)));
	vec3 N = normalize(vec3(u_InstanceBuffer.Transform * vec4(InNorm, 0.0)));

	FragTBN = mat3(T, B, N);

	FragNorm = mat3(transpose(inverse(u_InstanceBuffer.Transform))) * InNorm;
	FragTexC = InTexC;
	
	vec4 Pos = u_InstanceBuffer.Transform * vec4(InPos, 1.0);
	FragPos = vec3(Pos);
	
	gl_Position = u_CameraBuffer.projection * u_CameraBuffer.view * Pos;
}

#shader FRAGMENT
#version 450 core

layout(location = 0) in vec3 FragPos;
layout(location = 1) in vec3 FragNorm;
layout(location = 2) in vec2 FragTexC;
layout(location = 3) in mat3 FragTBN;

layout(location = 0) out vec4 OutPosition;
layout(location = 1) out vec4 OutNormal;
layout(location = 2) out vec4 OutBaseColor;

layout(set = 2, binding = 0) uniform MaterialBuffer
{
	vec4 baseColor;
	vec3 emissive; float n0;
	vec3 specular; 
	
	int pbrType;
	int useBaseColorMap;
	int useSurfaceMap;
	int useEmissiveMap;
	int useNormalMap;
	int useOculsionMap;

	float metalness, roughness, glossiness;
} u_MaterialBuffer;

layout(set = 2, binding = 1) uniform sampler2D baseColorMap;
layout(set = 2, binding = 2) uniform sampler2D normalMap;

void main()
{
	OutPosition = vec4(FragPos,1.0);

	if(u_MaterialBuffer.useNormalMap > -1)
	{
		vec3 norm = texture(normalMap,FragTexC).rgb;
		norm = norm * 2.0 - 1.0;
		norm = normalize(FragTBN * norm);

		OutNormal = vec4(norm, 1.0);
	}
	else
	{
		OutNormal = vec4(normalize(FragNorm), 1.0);
	}

	if(u_MaterialBuffer.useBaseColorMap > -1)
		OutBaseColor = texture(baseColorMap, FragTexC) * u_MaterialBuffer.baseColor;
	else
		OutBaseColor = texture(baseColorMap, FragTexC);
}