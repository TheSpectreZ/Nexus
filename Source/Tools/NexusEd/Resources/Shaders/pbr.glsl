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

layout(location = 0) out vec4 OutColor;

layout(set = 0, binding = 0) uniform CameraBuffer
{
	mat4 projection;
	mat4 view;
	vec3 position;
	float null;
} u_CameraBuffer;

struct PointLight
{
	vec3 position; float size;
	vec3 color; float intensity;
	vec3 null; float falloff;
	vec4 n;
};

layout(set = 0, binding = 1) uniform sceneBuffer
{
	vec3 SceneLightDirection;
	float pointLightCount;
	vec4 SceneLightColor;

	vec4 n1,n2;

	PointLight lights[10];
} m_sceneBuffer;

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
} m_MaterialBuffer;

layout(set = 2, binding = 1) uniform sampler2D baseColorMap;
layout(set = 2, binding = 2) uniform sampler2D surfaceMap;
layout(set = 2, binding = 3) uniform sampler2D normalMap;
layout(set = 2, binding = 4) uniform sampler2D occulsionMap;

const float c_MinRoughness = 0.04;

const int PBR_WORKFLOW_METALLIC_ROUGHNESS = 1;
const int PBR_WORKFLOW_SPECULAR_GLOSINESS = 2;

float convertMetallic(vec3 diffuse, vec3 specular, float maxSpecular) {
	float perceivedDiffuse = sqrt(0.299 * diffuse.r * diffuse.r + 0.587 * diffuse.g * diffuse.g + 0.114 * diffuse.b * diffuse.b);
	float perceivedSpecular = sqrt(0.299 * specular.r * specular.r + 0.587 * specular.g * specular.g + 0.114 * specular.b * specular.b);
	if (perceivedSpecular < c_MinRoughness) {
		return 0.0;
	}
	float a = c_MinRoughness;
	float b = perceivedDiffuse * (1.0 - maxSpecular) / (1.0 - c_MinRoughness) + perceivedSpecular - 2.0 * c_MinRoughness;
	float c = c_MinRoughness - perceivedSpecular;
	float D = max(b * b - 4.0 * a * c, 0.0);
	return clamp((-b + sqrt(D)) / (2.0 * a), 0.0, 1.0);
}

vec3 GetNormal()
{
	if (m_MaterialBuffer.useNormalMap > 1)
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
	float roughness;
	float metallic;
	vec3 diffuseColor;
	vec4 baseColor;

	vec3 f0 = vec3(0.04);

	if (m_MaterialBuffer.useBaseColorMap > -1)
		baseColor = texture(baseColorMap, FragTexC) * m_MaterialBuffer.baseColor;
	else
		baseColor =  m_MaterialBuffer.baseColor;
	
	if (m_MaterialBuffer.pbrType == PBR_WORKFLOW_METALLIC_ROUGHNESS)
	{
		roughness = m_MaterialBuffer.roughness;
		metallic = m_MaterialBuffer.metalness;

		if (m_MaterialBuffer.useSurfaceMap > -1) 
		{
			vec4 mrSample = texture(surfaceMap, FragTexC);
			roughness = mrSample.g * roughness;
			metallic = mrSample.b * metallic;
		} 
		else 
		{
			roughness = clamp(roughness, c_MinRoughness, 1.0);
			metallic = clamp(metallic, 0.0, 1.0);
		}
	}

	if (m_MaterialBuffer.pbrType == PBR_WORKFLOW_SPECULAR_GLOSINESS) 
	{
		vec3 specular;
		if (m_MaterialBuffer.useSurfaceMap > -1) 
		{
			vec4 sgSample = texture(surfaceMap,FragTexC); 
			roughness = 1.0 - sgSample.a;
			specular = sgSample.rgb;
		}
		else
		{
			roughness = 0.0;
		}

		diffuseColor = baseColor.rgb;
		
		float maxSpecular = max(max(specular.r, specular.g), specular.b);

		metallic = convertMetallic(diffuseColor.rgb, specular, maxSpecular);

		vec3 baseColorDiffusePart = diffuseColor.rgb * ((1.0 - maxSpecular) / (1 - c_MinRoughness) / max(1 - metallic, Epsilon)) * m_MaterialBuffer.baseColor.rgb;
		vec3 baseColorSpecularPart = specular - (vec3(c_MinRoughness) * (1 - metallic) * (1 / max(metallic, Epsilon))) * m_MaterialBuffer.specular.rgb;
		baseColor = vec4(mix(baseColorDiffusePart, baseColorSpecularPart, metallic * metallic), baseColor.a);

	}

	diffuseColor = baseColor.rgb * (vec3(1.0) - f0);
	diffuseColor *= 1.0 - metallic;

	vec3 norm = GetNormal();

	vec3 viewDir = normalize(u_CameraBuffer.position - FragPos);

	vec3 F0 = mix(Fdielectric, diffuseColor, metallic);

	vec3 result = vec3(0.0);

	// Point Lights
	// Check it light count is less that 10
	for (int i = 0; i < m_sceneBuffer.pointLightCount; i++)
	{
		vec3 lightDir = m_sceneBuffer.lights[i].position - FragPos;
		vec3 lightVector = normalize(lightDir);

		vec3 halfDir = normalize(lightVector + viewDir);

		vec3 Fresnel = Calculate_Fresnel(F0, viewDir, halfDir);

		vec3 diffuse = Calculate_Lambertian_DiffuseComponent(diffuseColor, Fresnel);
		vec3 specular = Calculate_CookTorrance_SpecularComponent(viewDir, halfDir, lightVector, norm, Fresnel, roughness);

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
		vec3 halfDir = normalize(m_sceneBuffer.SceneLightDirection + viewDir);

		vec3 Fresnel = Calculate_Fresnel(F0, viewDir, halfDir);

		vec3 diffuse = Calculate_Lambertian_DiffuseComponent(diffuseColor, Fresnel);
		vec3 specular = Calculate_CookTorrance_SpecularComponent(viewDir, halfDir, m_sceneBuffer.SceneLightDirection.rgb, norm, Fresnel, roughness);

		result += (diffuse + specular) * m_sceneBuffer.SceneLightColor.rgb * max(dot(m_sceneBuffer.SceneLightDirection.rgb, norm), 0.0);
	}

	const float u_OcclusionStrength = 1.0f;
	if (m_MaterialBuffer.useOculsionMap > -1)
	{
		float ao = texture(occulsionMap,FragTexC).r;
		result = mix(result, result * ao, u_OcclusionStrength);
	}

	// Tone Mapping
	result = vec3(1.0) - exp(-result * Exposure);

	// Gamma Correction
	result = pow(result, vec3(1.0 / Gamma));

	OutColor = vec4(result, 1.0);
}
