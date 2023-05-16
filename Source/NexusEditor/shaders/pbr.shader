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

vec3 CalculateAmbientColor(vec3 lightCol, float ambientStrength)
{
	return ambientStrength * lightCol;
}

vec3 CalculateDiffuseColor(vec3 lightCol, vec3 lightDir, vec3 normal)
{
	float diff = max(dot(normal, lightDir), 0.0);
	return diff * lightCol;
}

vec3 CalculateSpecularColor(vec3 lightCol, vec3 lightDir, vec3 normal,vec3 view,float specular,float shine)
{
	vec3 ref = reflect(-lightDir, normal);

	float spec = pow(max(dot(view, ref), 0.0), shine);
	return specular * spec * lightCol;
}

void main()
{
	vec3 albedo = GetMaterialColor();
	vec3 viewDir = normalize(m_sceneBuffer.position - FragPos);
	vec3 norm = normalize(FragNorm);

	vec3 result = vec3(0.0);

	// Point Lights
	// Check it light count is less that 10
	for (int i = 0; i < m_sceneBuffer.pointLightCount; i++)
	{
		vec3 lightDir = m_sceneBuffer.lights[i].position - FragPos;
	
		vec3 ambient = CalculateAmbientColor(m_sceneBuffer.lights[i].color, 0.1);
		vec3 diffuse = CalculateDiffuseColor(m_sceneBuffer.lights[i].color, lightDir, norm);
		vec3 specular = CalculateSpecularColor(m_sceneBuffer.lights[i].color, lightDir, norm, viewDir, 0.5, 32);
	
		result += (ambient + diffuse + specular) * albedo;
	}

	// Directional Light
	{
		vec3 ambient = CalculateAmbientColor(m_sceneBuffer.SceneLightColor.rgb, 0.1);
		vec3 diffuse = CalculateDiffuseColor(m_sceneBuffer.SceneLightColor.rgb, m_sceneBuffer.SceneLightDirection.rgb, norm);
		vec3 specular = CalculateSpecularColor(m_sceneBuffer.SceneLightColor.rgb, m_sceneBuffer.SceneLightDirection.rgb, norm, viewDir, 0.5, 32.0);

		result += (ambient + diffuse + specular) * albedo;
	}

	OutColor = vec4(result, 1.0);
}
