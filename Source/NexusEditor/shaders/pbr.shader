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
	vec3 T = normalize(vec3(u_InstanceBuffer.Transform * vec4(InTang, 0.0)));
	vec3 B = normalize(vec3(u_InstanceBuffer.Transform * vec4(InBiTang, 0.0)));
	vec3 N = normalize(vec3(u_InstanceBuffer.Transform * vec4(InNorm, 0.0)));

	FragTBN = mat3(T, B, N);

	FragNorm = mat3(transpose(inverse(u_InstanceBuffer.Transform))) * InNorm;
	FragTexC = InTexC;
	
	vec4 Pos = u_InstanceBuffer.Transform * vec4(InPos, 1.0);
	FragPos = vec3(Pos);
	gl_Position = u_SceneBuffer.projection * u_SceneBuffer.view * Pos;
}

#shader FRAGMENT
#version 450 core

layout(location = 0) in vec3 FragPos;
layout(location = 1) in vec3 FragNorm;
layout(location = 2) in vec2 FragTexC;
layout(location = 3) in mat3 FragTBN;

layout(location = 0) out vec4 OutColor;

struct PointLight
{
	vec3 position; float size;
	vec3 color; float intensity;
	vec3 null; float falloff;
};

layout(set = 0, binding = 1) uniform sceneBuffer
{
	vec3 position; float pointLightCount;
	vec4 SceneLightDirection;
	vec4 SceneLightColor;
	vec4 null;

	PointLight lights[10];
} m_sceneBuffer;

layout(set = 2, binding = 0) uniform MaterialBuffer
{
	vec4 AlbedoColor;
	float roughness;
	float metalness;
	float albedoTexCoord;
	float mrTexCoord;

	float useMR;
	float useAlbedo;
	float useNormal;

	float null;
	vec4 null2;

} m_MaterialBuffer;

layout(set = 2, binding = 1) uniform sampler2D albedoMap;
layout(set = 2, binding = 2) uniform sampler2D metallicRoughnessMap;
layout(set = 2, binding = 3) uniform sampler2D normalMap;

vec3 GetMaterialColor()
{
	if (m_MaterialBuffer.useAlbedo == 1.0)
		return texture(albedoMap, FragTexC).rgb;
	else
		return m_MaterialBuffer.AlbedoColor.rgb;
}

vec2 GetMetallicRoughness()
{
	if (m_MaterialBuffer.useMR == 1.0)
		return texture(metallicRoughnessMap, FragTexC).rg;
	else
		return vec2(m_MaterialBuffer.metalness, m_MaterialBuffer.roughness);
}

vec3 GetNormal()
{
	if (m_MaterialBuffer.useNormal == 1.0)
	{
		vec3 normal = texture(normalMap, FragTexC).rgb;
		normal = normal * 2.0 - 1.0;
		normal = normalize(FragTBN * normal);
		return normal;
	}
	else
		return normalize(FragNorm);
}

const float Gamma = 2.2;
const float Exposure = 1.0;
const float PI = 3.141592;
const float Epsilon = 0.00001;
const vec3 Fdielectric = vec3(0.01); // Constant normal Incidence Frensel Factor for all Dielectrics

float ndfGGX(float cosLh, float roughness)
{
	float alpha = roughness * roughness;
	float alphaSq = alpha * alpha;

	float denom = (cosLh * cosLh) * (alphaSq - 1.0) + 1.0;
	return alphaSq / (PI * denom * denom);
}

// Single term for separable Schlick-GGX below.
float gaSchlickG1(float cosTheta, float k)
{
	return cosTheta / (cosTheta * (1.0 - k) + k);
}

// Schlick-GGX approximation of geometric attenuation function using Smith's method.
float gaSchlickGGX(float cosLi, float cosLo, float roughness)
{
	float r = roughness + 1.0;
	float k = (r * r) / 8.0; // Epic suggests using this roughness remapping for analytic lights.
	return gaSchlickG1(cosLi, k) * gaSchlickG1(cosLo, k);
}

// Shlick's approximation of the Fresnel factor.
vec3 fresnelSchlick(vec3 F0, float cosTheta)
{
	return F0 + (vec3(1.0) - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 CalculateBRDF(vec3 F0, vec3 albedo, float cosLhLo, float cosLh, float cosLi, float cosLo, float roughness, float metalness)
{
	vec3 F = fresnelSchlick(F0, max(0.0, cosLhLo));
	float D = ndfGGX(cosLh, roughness);
	float G = gaSchlickGGX(cosLi, cosLo, roughness);

	vec3 kd = mix(vec3(1.0) - F, vec3(0.0), metalness);

	vec3 diffuseBRDF = kd * albedo;
	vec3 specularBRDF = (F * D * G) / max(Epsilon, 4.0 * cosLi * cosLo);

	return diffuseBRDF + specularBRDF;
}

void main()
{
	vec3 albedo = GetMaterialColor();
	vec2 MR = GetMetallicRoughness();
	vec3 N = GetNormal();

	float metalness = MR.r;
	float roughness = MR.g;

	vec3 F0 = mix(Fdielectric, albedo, metalness);
	
	vec3 Lo = normalize(m_sceneBuffer.position - FragPos);
	float cosLo = max(0.0, dot(N, Lo));


	vec3 Lr = 2.0 * cosLo * N - Lo;

	vec3 directLighting = vec3(0);
	
	// PointLights
	for (int i = 0; i < m_sceneBuffer.pointLightCount; ++i)
	{
		vec3 Li = m_sceneBuffer.lights[i].position - FragPos;
		vec3 Lradiance = m_sceneBuffer.lights[i].color;

		vec3 Lh = normalize(Li + Lo);

		float cosLhLo = max(0.0, dot(Lh, Lo));
		float cosLi = max(0.0, dot(N, Li));
		float cosLh = max(0.0, dot(N, Lh));

		vec3 BRDF = CalculateBRDF(F0, albedo, cosLhLo, cosLh, cosLi, cosLo, roughness, metalness);
		
		float attenuation = 0.0;
		{
			float d = length(Li);
			float s = d / m_sceneBuffer.lights[i].size;
			if (s < 1.0)
			{
				float s2 = s * s;
				float s3 = (1 - s2) * (1 - s2);

				attenuation = m_sceneBuffer.lights[i].intensity * s3 / (1 + m_sceneBuffer.lights[i].falloff * s);
			}
		}

		directLighting += BRDF * Lradiance * attenuation  * cosLi;
	}

	// Directional Light
	{
		vec3 Li = normalize(m_sceneBuffer.SceneLightDirection.rgb);
		vec3 Lradiance = m_sceneBuffer.SceneLightColor.rgb;

		vec3 Lh = normalize(Li + Lo);

		float cosLhLo = max(0.0, dot(Lh, Lo));
		float cosLi = max(0.0, dot(N, Li));
		float cosLh = max(0.0, dot(N, Lh));

		vec3 BRDF = CalculateBRDF(F0, albedo, cosLhLo, cosLh, cosLi, cosLo, roughness, metalness);

		directLighting += BRDF * Lradiance * cosLi;
	}
	
	// Final fragment color.
	OutColor = vec4(directLighting , 1.0);
}