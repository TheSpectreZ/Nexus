#shader VERTEX
#version 450 core

layout(location = 0) in vec3 InPos;
layout(location = 1) in vec3 InNorm;
layout(location = 2) in vec2 InTexC;

layout(location = 0) out vec3 FragPos;
layout(location = 1) out vec3 FragNorm;
layout(location = 2) out vec2 FragTexC;

layout(set = 0, binding = 0) uniform SceneBuffer
{
	mat4 projection;
	mat4 view;
} u_SceneBuffer;

layout(set = 2, binding = 0) uniform InstanceBuffer
{
	mat4 Transform;
} u_InstanceBuffer;

void main()
{
	vec4 Pos = u_InstanceBuffer.Transform * vec4(InPos, 1.0);

	FragPos = vec3(Pos);
	FragNorm = InNorm;
	FragTexC = InTexC;

	gl_Position = u_SceneBuffer.projection * u_SceneBuffer.view * Pos;
}

#shader FRAGMENT
#version 450 core

const float PI = 3.141592;
const float Epsilon = 0.00001;
const vec3 Fdielectrics = vec3(0.01); // Constant normal Incidence Frensel Factor for all Dielectrics

layout(location = 0) in vec3 FragPos;
layout(location = 1) in vec3 FragNorm;
layout(location = 2) in vec2 FragTexC;

layout(location = 0) out vec4 OutColor;

layout(set = 0, binding = 1) uniform cameraBuffer
{
	vec3 position;
} u_cameraBuffer;

const int NumLights = 3;

struct Light
{
	vec3 direction;
	vec3 radiance;
};

layout(set = 0, binding = 2) uniform LightBuffer
{
	Light lights[NumLights];
} u_LightBuffer;

layout(set = 1, binding = 0) uniform sampler2D albedoMap;
layout(set = 1, binding = 1) uniform sampler2D normalMap;
layout(set = 1, binding = 2) uniform sampler2D metalnessMap;
layout(set = 1, binding = 3) uniform sampler2D roughnessMap;

float GGX_NormalDistribution(float cosLH, float roughness)
{
	float alphaSq = roughness * roughness * roughness * roughness;
	float denom = (cosLH * cosLH) * (alphaSq - 1.0) + 1.0;

	return alphaSq / (PI * denom * denom);
}

float Schlick_GGC_Helper(float cosTheta, float k)
{
	return cosTheta / (cosTheta * (1.0 - k) + k);
}

float Schlick_GGX_GeometricAttenuation(float cosLi, float cosLo, float roughness)
{
	float r = roughness + 1.0;
	float k = (r * r) / 8.0;

	return Schlick_GGC_Helper(cosLi, k) * Schlick_GGC_Helper(cosLo, k);
}

vec3 Schlick_FresnelFactor(vec3 F, float cosTheta)
{
	return F + (vec3(1.0) - F) * pow(1.0 - cosTheta, 5.0);
}

void main()
{
	vec3 albedo = texture(albedoMap, FragTexC).rgb;
	vec3 normal = texture(normalMap, FragTexC).rgb;
	float metalness = texture(metalnessMap, FragTexC).r;
	float roughness = texture(roughnessMap, FragTexC).r;

	vec3 Lo = normalize(u_cameraBuffer.position - FragPos);
	vec3 N = normalize(2.0 * normal - 1.0);

	//N = normalize(tangentBasis * N);

	float cosLo = max(0.0, dot(N, Lo));

	vec3 Lr = 2.0 * cosLo * N - Lo;
	vec3 F = mix(Fdielectrics, albedo, metalness);

	vec3 directLighting = vec3(0.0);
	for (int i = 0; i < NumLights; i++)
	{
		vec3 Li = u_LightBuffer.lights[i].direction;
		vec3 Lr = u_LightBuffer.lights[i].radiance;

		vec3 Lh = normalize(Li + Lo);

		float cosLi = max(0.0, dot(N, Li));
		float cosLh = max(0.0, dot(N, Lh));
		float cosLhLo = max(0.0, dot(Lh, Lo));

		vec3 F = Schlick_FresnelFactor(F, cosLhLo);
		
		float D = GGX_NormalDistribution(cosLh, roughness);
		float G = Schlick_GGX_GeometricAttenuation(cosLi, cosLo, roughness);

		vec3 kd = mix(vec3(1.0) - F, vec3(0.0), metalness);

		vec3 diffuseBRDF = kd * albedo;
		vec3 specularBRDF = (F * D * G) / max(Epsilon, 4.0 * cosLi * cosLo);

		directLighting += (diffuseBRDF + specularBRDF) * Lr * cosLi;
	}

	OutColor = vec4(directLighting, 1.0);
}
