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

layout(location = 0) in vec3 FragPos;
layout(location = 1) in vec3 FragNorm;
layout(location = 2) in vec2 FragTexC0;
layout(location = 3) in vec2 FragTexC1;

layout(location = 0) out vec4 OutColor;

struct PointLight
{
	vec3 position; float nul1;
	vec3 color; float nul2;
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

	vec2 null1;
	vec4 null2;

} m_MaterialBuffer;

layout(set = 2, binding = 1) uniform sampler2D albedoMap;
layout(set = 2, binding = 2) uniform sampler2D metallicRoughnessMap;

vec3 GetMaterialColor()
{
	if (m_MaterialBuffer.useAlbedo == 1.0)
	{
		vec2 texCoord = (m_MaterialBuffer.albedoTexCoord == 0.0) ? FragTexC0 : FragTexC1;
		return texture(albedoMap, texCoord).rgb;
	}
	else
		return m_MaterialBuffer.AlbedoColor.rgb;
}

vec2 GetMetallicRoughness()
{
	if (m_MaterialBuffer.useMR == 1.0)
	{
		vec2 texCoord = (m_MaterialBuffer.mrTexCoord == 0.0) ? FragTexC0 : FragTexC1;
		return texture(metallicRoughnessMap, texCoord).rg;
	}
	else
		return vec2(m_MaterialBuffer.metalness, m_MaterialBuffer.roughness);
}

const float Gamma = 2.2;
const float PI = 3.141592;
const float Epsilon = 0.00001;
const vec3 Fdielectric = vec3(0.01); // Constant normal Incidence Frensel Factor for all Dielectrics

vec3 Calculate_Fresnel(vec3 F0, vec3 viewDir, vec3 halfDir)
{
	return F0 + (vec3(1) - F0) * pow((1 - max(dot(viewDir, halfDir), 0.0)), 5.0);
}

vec3 Calculate_Lambertian_DiffuseComponent(vec3 materialColor,vec3 Fresnel)
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
	float cosNV = max(dot(Normal,  viewDir), 0.0);

	return GGXschilck_SingleTerm(cosNL, k) * GGXschilck_SingleTerm(cosNV, k);
}

vec3 Calculate_CookTorrance_SpecularComponent(vec3 viewDir, vec3 halfDir, vec3 lightDir, vec3 normal, vec3 Fresnel, float roughness)
{
	float D = GGX_NormalDistribution(roughness, normal, halfDir);
	float G = GGXschick_GeometryShading(lightDir, normal, viewDir, roughness);

	float cosVN = max(dot(viewDir, normal), 0.0);
	float cosLN = max(dot(lightDir, normal), 0.0);

	vec3 num = D * G * Fresnel;
	float den = 4 * cosVN * cosLN;

	return num / den;
}

void main()
{
	vec3 albedo = GetMaterialColor();
	vec2 metallicRoughness = GetMetallicRoughness();

	vec3 viewDir = normalize(m_sceneBuffer.position - FragPos);
	vec3 norm = normalize(FragNorm);
	
	vec3 F0 = mix(Fdielectric, albedo, metallicRoughness.r);

	vec3 result = vec3(0.0);

	// Point Lights
	// Check it light count is less that 10
	for (int i = 0; i < m_sceneBuffer.pointLightCount; i++)
	{
		vec3 lightDir = m_sceneBuffer.lights[i].position - FragPos;
		vec3 halfDir = normalize(lightDir + viewDir);
		
		vec3 Fresnel = Calculate_Fresnel(F0, viewDir, halfDir);

		vec3 diffuse = Calculate_Lambertian_DiffuseComponent(albedo, Fresnel);
		vec3 specular = Calculate_CookTorrance_SpecularComponent(viewDir, halfDir, lightDir,norm, Fresnel, metallicRoughness.g);

		result += (diffuse + specular) * m_sceneBuffer.lights[i].color * max(dot(lightDir, norm), 0.0);
	}

	// Directional Light
	{
		vec3 halfDir = normalize(m_sceneBuffer.SceneLightDirection.rgb + viewDir);

		vec3 Fresnel = Calculate_Fresnel(F0, viewDir, halfDir);
		
		vec3 diffuse = Calculate_Lambertian_DiffuseComponent(albedo, Fresnel);
		vec3 specular = Calculate_CookTorrance_SpecularComponent(viewDir, halfDir, m_sceneBuffer.SceneLightDirection.rgb, norm, Fresnel, metallicRoughness.g);

		result += (diffuse + specular) * m_sceneBuffer.SceneLightColor.rgb * max(dot(m_sceneBuffer.SceneLightDirection.rgb, norm), 0.0);
	}

	// Gamma Correction
	result = pow(result, vec3(1.0 / Gamma));

	OutColor = vec4(result, 1.0);
}
