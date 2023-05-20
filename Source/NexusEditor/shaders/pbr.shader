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

vec3 Calculate_Fresnel(vec3 F0, vec3 viewDir, vec3 halfDir)
{
	return F0 + (vec3(1) - F0) * pow((1 - max(dot(viewDir, halfDir), 0.0)), 5.0);
}

vec3 Calculate_Lambertian_DiffuseComponent(vec3 materialColor, vec3 Fresnel)
{
	vec3 diff = materialColor / PI;
	return (vec3(1.0) - Fresnel) * diff;
}

float GGX_NormalDistribution(float roughness, vec3 normal, vec3 halfDir)
{
	float alpha = roughness * roughness;
	float alphaSq = alpha * alpha;
	float cosNH = max(dot(normal, halfDir), 0.0);

	float den = pow((pow(cosNH, 2.0) * (alphaSq - 1) + 1), 2.0) * PI;

	return alphaSq / den;
}

float GGXschilck_SingleTerm(float cosNX, float k)
{
	return cosNX / (cosNX * (1 - k) + k);
}

float GGXschick_GeometryShading(vec3 lightDir, vec3 Normal, vec3 viewDir, float roughness)
{
	float k = pow(roughness, 2.0) / 2;
	float cosNL = max(dot(Normal, lightDir), 0.0);
	float cosNV = max(dot(Normal, viewDir), 0.0);

	return GGXschilck_SingleTerm(cosNL, k) * GGXschilck_SingleTerm(cosNV, k);
}

vec3 Calculate_CookTorrance_SpecularComponent(vec3 viewDir, vec3 halfDir, vec3 lightDir, vec3 normal, vec3 Fresnel, float roughness)
{
	float D = GGX_NormalDistribution(roughness, normal, halfDir);
	float G = GGXschick_GeometryShading(lightDir, normal, viewDir, roughness);

	float cosVN = max(dot(viewDir, normal), 0.0);
	float cosLN = max(dot(lightDir, normal), 0.0);

	vec3 num = D * G * Fresnel;
	float den = max(Epsilon, 4 * cosVN * cosLN);

	return num / den;
}

void main()
{
	vec3 albedo = GetMaterialColor();
	vec2 metallicRoughness = GetMetallicRoughness();
	vec3 norm = GetNormal();

	vec3 viewDir = normalize(m_sceneBuffer.position - FragPos);

	vec3 F0 = mix(Fdielectric, albedo, metallicRoughness.r);

	vec3 result = vec3(0.0);

	// Point Lights
	// Check it light count is less that 10
	for (int i = 0; i < m_sceneBuffer.pointLightCount; i++)
	{
		vec3 lightDir = m_sceneBuffer.lights[i].position - FragPos;
		vec3 lightVector = normalize(lightDir);

		vec3 halfDir = normalize(lightVector + viewDir);

		vec3 Fresnel = Calculate_Fresnel(F0, viewDir, halfDir);

		vec3 diffuse = Calculate_Lambertian_DiffuseComponent(albedo, Fresnel);
		vec3 specular = Calculate_CookTorrance_SpecularComponent(viewDir, halfDir, lightVector, norm, Fresnel, metallicRoughness.g);

		float attenuation = 0.0;
		{
			float d = length(lightDir);
			float s = d / m_sceneBuffer.lights[i].size;
			if (s < 1.0)
			{
				float s2 = s * s;
				float s3 = (1 - s2) * (1 - s2);

				attenuation = m_sceneBuffer.lights[i].intensity * s3 / (1 + m_sceneBuffer.lights[i].falloff * s);
			}
		}
		vec3 lightColor = m_sceneBuffer.lights[i].color * m_sceneBuffer.lights[i].intensity * attenuation;

		result += (diffuse + specular) * lightColor * max(dot(lightDir, norm), 0.0);
	}

	// Directional Light
	{
		vec3 halfDir = normalize(m_sceneBuffer.SceneLightDirection.rgb + viewDir);

		vec3 Fresnel = Calculate_Fresnel(F0, viewDir, halfDir);

		vec3 diffuse = Calculate_Lambertian_DiffuseComponent(albedo, Fresnel);
		vec3 specular = Calculate_CookTorrance_SpecularComponent(viewDir, halfDir, m_sceneBuffer.SceneLightDirection.rgb, norm, Fresnel, metallicRoughness.g);

		result += (diffuse + specular) * m_sceneBuffer.SceneLightColor.rgb * max(dot(m_sceneBuffer.SceneLightDirection.rgb, norm), 0.0);
	}

	// Tone Mapping
	result = vec3(1.0) - exp(-result * Exposure);

	// Gamma Correction
	result = pow(result, vec3(1.0 / Gamma));

	OutColor = vec4(result, 1.0);
}
