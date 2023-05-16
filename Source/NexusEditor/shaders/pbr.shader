#shader VERTEX
#version 450 core

layout(location = 0) in vec3 InPos;
layout(location = 1) in vec3 InNorm;
layout(location = 2) in vec2 InTexC0;
layout(location = 3) in vec2 InTexC1;

layout(location = 0) out vec3 FragPos;
layout(location = 1) out vec3 FragNorm;
layout(location = 2) out vec2 FragTexC0;
layout(location = 3) out vec2 FragTexC1;

layout(set = 0, binding = 0) uniform SceneBuffer
{
	mat4 projection;
	mat4 view;
} u_SceneBuffer;

layout(set = 1, binding = 0) uniform InstanceBuffer
{
	mat4 Transform;
} u_InstanceBuffer;

void main()
{
	vec4 Pos = u_InstanceBuffer.Transform * vec4(InPos, 1.0);

	FragPos = vec3(Pos);
	FragNorm = mat3(transpose(inverse(u_InstanceBuffer.Transform))) * InNorm;
	FragTexC0 = InTexC0;
	FragTexC1 = InTexC1;

	gl_Position = u_SceneBuffer.projection * u_SceneBuffer.view * Pos;
}

#shader FRAGMENT
#version 450 core

const float PI = 3.141592;
const float Epsilon = 0.00001;
const vec3 Fdielectrics = vec3(0.01); // Constant normal Incidence Frensel Factor for all Dielectrics

layout(location = 0) in vec3 FragPos;
layout(location = 1) in vec3 FragNorm;
layout(location = 2) in vec2 FragTexC0;
layout(location = 3) in vec2 FragTexC1;

layout(location = 0) out vec4 OutColor;

layout(set = 0, binding = 1) uniform cameraBuffer
{
	vec4 position;
	vec4 n1, n2, n3;
} m_cameraBuffer;


layout(set = 2, binding = 0) uniform MaterialBuffer
{
	vec4 AlbedoColor;
	float roughness;
	float metalness;
	float albedoTexCoord;
	float mrTexCoord;

	float useMR;
	float useAlbedo;

	vec2 null1;
	vec4 null2;

} m_MaterialBuffer;

layout(set = 2, binding = 1) uniform sampler2D albedoMap;
layout(set = 2, binding = 2) uniform sampler2D metallicRoughnessMap;

vec3 LightDir = vec3(1.0, 1.0, 1.0);
vec3 LightColor = vec3(1.0, 1.0, 1.0);

void main()
{
	vec3 albedo = vec3(0.0);
	
	if (m_MaterialBuffer.useAlbedo == 1.0)
	{
		vec2 texCoord = (m_MaterialBuffer.albedoTexCoord == 0.0) ? FragTexC0 : FragTexC1;
		albedo = texture(albedoMap, texCoord).rgb;
	}
	else
		albedo = m_MaterialBuffer.AlbedoColor.rgb;

	// Ambient
	vec3 ambColor = 0.1 * LightColor;

	// Diffuse
	vec3 norm = normalize(FragNorm);
	vec3 dir = normalize(LightDir);

	float diff = max(dot(norm, dir), 0.0);
	vec3 diffColor = diff * LightColor;

	// Specular
	vec3 viewDir = normalize(m_cameraBuffer.position.xyz - FragPos);
	vec3 refDir = reflect(-dir, norm);

	float spec = pow(max(dot(viewDir, refDir), 0.0), 32);
	vec3 specColor = 0.5 * spec * LightColor;

	vec3 result = (ambColor + diffColor + specColor) * albedo;

	OutColor = vec4(result, 1.0);
}
