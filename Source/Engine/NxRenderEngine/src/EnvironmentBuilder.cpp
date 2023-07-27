#include "NxRenderEngine/EnvironmentBuilder.h"
#include "NxRenderEngine/Renderer.h"
#include "NxRenderEngine/ResourcePool.h"
#include "NxCore/Logger.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

Nexus::EnvironmentBuilder* Nexus::EnvironmentBuilder::s_Instance = nullptr;

void Nexus::EnvironmentBuilder::Initialize()
{
	s_Instance = new EnvironmentBuilder;
	s_Instance->m_queue = Module::Renderer::Get()->GetCommandQueue();

	s_Instance->m_brdfLut.Build("Resources/Shaders/compute/GenBRDFLut.glsl");
	s_Instance->m_Irradiance.Build("Resources/Shaders/compute/GenIrradiance.glsl");
	s_Instance->m_Specular.Build("Resources/Shaders/compute/GenSpecular.glsl");
	s_Instance->m_Eq2Cube.Build("Resources/Shaders/compute/GenCubeMap.glsl");

	// BRDF LUT
	{
		const uint32_t brdfSize = 256;

		TextureSpecification specs{};
		specs.extent = { brdfSize,brdfSize };
		specs.pixels = nullptr;
		specs.format = TextureFormat::RG16_SFLOAT;
		specs.type = TextureType::TwoDim;
		specs.usage = TextureUsage::StorageWrite;

		s_Instance->m_BRDFLutTex = GraphicsInterface::CreateTexture(specs);

		ImageHandle handle{};
		handle.texture = s_Instance->m_BRDFLutTex;
		handle.sampler = nullptr;
		handle.set = 0;
		handle.binding = 0;
		handle.Type = ShaderResourceType::StorageImage;
		s_Instance->m_brdfLut.shader->BindTextureWithResourceHeap(s_Instance->m_brdfLut.heap, handle);

		uint32_t numWorkGroupsX = (brdfSize + 16 - 1) / 16;
		uint32_t numWorkGroupsY = (brdfSize + 16 - 1) / 16;

		s_Instance->m_queue->StartComputeQueue();
		s_Instance->m_queue->BindShaderResourceHeap(s_Instance->m_brdfLut.shader, s_Instance->m_brdfLut.heap, PipelineBindPoint::Compute);
		s_Instance->m_queue->BindPipeline(s_Instance->m_brdfLut.pipeline);
		s_Instance->m_queue->Dispatch(numWorkGroupsX, numWorkGroupsY, 1);
		s_Instance->m_queue->FlushComputeQueue();

		s_Instance->m_BRDFLutTex->PrepareForRender();
	}
}

void Nexus::EnvironmentBuilder::Shutdown()
{
	delete s_Instance;
}

Nexus::Ref<Nexus::Environment> Nexus::EnvironmentBuilder::Build(const std::string& hdrPath, UUID Id)
{
	int w, h, c;
	float* pixels = stbi_loadf(hdrPath.c_str(), &w, &h, &c, 4);

	if (!pixels)
	{
		NEXUS_LOG("Renderer", "Failed to Load HDR Environment - %s", stbi_failure_reason());
		return nullptr;
	}

	Ref<Environment> env = ResourcePool::Get()->AllocateEnvironment(Id);

	Ref<Texture> equEnvMap;
	{
		TextureSpecification EnvSpecs{};
		EnvSpecs.extent = { (uint32_t)w,(uint32_t)h };
		EnvSpecs.format = TextureFormat::RGBA32_SFLOAT;
		EnvSpecs.type = TextureType::TwoDim;
		EnvSpecs.usage = TextureUsage::ShaderSampled;
		EnvSpecs.pixels = pixels;
		EnvSpecs.now = true;

		equEnvMap = GraphicsInterface::CreateTexture(EnvSpecs);
	}

	const static uint32_t cubeMapSize = 1024;
	{
		TextureSpecification specs{};
		specs.extent = { cubeMapSize ,cubeMapSize };
		specs.format = TextureFormat::RGBA16_SFLOAT;
		specs.type = TextureType::Cube;
		specs.usage = TextureUsage::StorageWrite;
		specs.pixels = nullptr;

		env->envMap = GraphicsInterface::CreateTexture(specs);
	}
	
	// Generate Cubemap From Equirectangular Image
	
	ImageHandle handle;
	handle.set = 0;

	handle.texture = equEnvMap;
	handle.sampler = ResourcePool::Get()->GetSampler(11122);
	handle.binding = 0;
	handle.Type = ShaderResourceType::SampledImage;
	s_Instance->m_Eq2Cube.shader->BindTextureWithResourceHeap(s_Instance->m_Eq2Cube.heap, handle);

	handle.texture = env->envMap;
	handle.sampler = nullptr;
	handle.binding = 1;
	handle.Type = ShaderResourceType::StorageImage;	
	s_Instance->m_Eq2Cube.shader->BindTextureWithResourceHeap(s_Instance->m_Eq2Cube.heap, handle);

	uint32_t numWorkGroupsX = (cubeMapSize + 32 - 1) / 32;
	uint32_t numWorkGroupsY = (cubeMapSize + 32 - 1) / 32;

	s_Instance->m_queue->StartComputeQueue();
	s_Instance->m_queue->BindShaderResourceHeap(s_Instance->m_Eq2Cube.shader, s_Instance->m_Eq2Cube.heap, PipelineBindPoint::Compute);
	s_Instance->m_queue->BindPipeline(s_Instance->m_Eq2Cube.pipeline);
	s_Instance->m_queue->Dispatch(numWorkGroupsX, numWorkGroupsY, 6);
	s_Instance->m_queue->FlushComputeQueue();

	// Generate Irradiance

	env->envMap->PrepareForRender();

	const static uint32_t cMapSize = 32;
	
	handle.texture = env->envMap;
	handle.sampler = ResourcePool::Get()->GetSampler(11122);
	handle.binding = 0;
	handle.Type = ShaderResourceType::SampledImage;
	s_Instance->m_Irradiance.shader->BindTextureWithResourceHeap(s_Instance->m_Irradiance.heap, handle);

	{
		TextureSpecification specs{};
		specs.extent = { cMapSize ,cMapSize };
		specs.format = TextureFormat::RGBA16_SFLOAT;
		specs.type = TextureType::Cube;
		specs.usage = TextureUsage::StorageWrite;
		specs.pixels = nullptr;

		env->IrradianceMap = GraphicsInterface::CreateTexture(specs);
	}
	
	handle.texture = env->IrradianceMap;
	handle.sampler = nullptr;
	handle.binding = 1;
	handle.Type = ShaderResourceType::StorageImage;
	s_Instance->m_Irradiance.shader->BindTextureWithResourceHeap(s_Instance->m_Irradiance.heap, handle);

	numWorkGroupsX = (cMapSize + 32 - 1) / 32;
	numWorkGroupsY = (cMapSize + 32 - 1) / 32;

	s_Instance->m_queue->StartComputeQueue();
	s_Instance->m_queue->BindShaderResourceHeap(s_Instance->m_Irradiance.shader, s_Instance->m_Irradiance.heap, PipelineBindPoint::Compute);
	s_Instance->m_queue->BindPipeline(s_Instance->m_Irradiance.pipeline);
	s_Instance->m_queue->Dispatch(numWorkGroupsX, numWorkGroupsY, 6);
	s_Instance->m_queue->FlushComputeQueue();

	env->IrradianceMap->PrepareForRender();

	// Generate Prefiltered Specular

	handle.texture = env->envMap;
	handle.sampler = ResourcePool::Get()->GetSampler(11122);
	handle.binding = 0;
	handle.Type = ShaderResourceType::SampledImage;
	s_Instance->m_Specular.shader->BindTextureWithResourceHeap(s_Instance->m_Specular.heap, handle);
	
	uint32_t mipCount = (uint32_t)std::floor(std::log2(std::max(cMapSize, cMapSize))) + 1;
	{
		TextureSpecification specs{};
		specs.extent = { cMapSize ,cMapSize };
		specs.format = TextureFormat::RGBA16_SFLOAT;
		specs.type = TextureType::Cube;
		specs.usage = TextureUsage::StorageWrite;
		specs.pixels = nullptr;
		specs.mipCount = mipCount;
	
		env->specularMap = GraphicsInterface::CreateTexture(specs);
	}
	
	handle.texture = env->specularMap;
	handle.sampler = nullptr;
	handle.binding = 1;
	handle.Type = ShaderResourceType::StorageImage;
	
	for (uint32_t i = 0; i < mipCount; i++)
	{
		handle.miplevel = i;
		s_Instance->m_Specular.shader->BindTextureWithResourceHeap(s_Instance->m_Specular.heap, handle);
	}
	
	numWorkGroupsX = (cMapSize + 32 - 1) / 32;
	numWorkGroupsY = (cMapSize + 32 - 1) / 32;
	
	s_Instance->m_queue->StartComputeQueue();
	s_Instance->m_queue->BindShaderResourceHeap(s_Instance->m_Specular.shader, s_Instance->m_Specular.heap, PipelineBindPoint::Compute);
	s_Instance->m_queue->BindPipeline(s_Instance->m_Specular.pipeline);
	
	struct
	{
		int level;
		float roughness;
	} pushData;
	
	const float deltaRoughness = 1.0f / std::max(float(mipCount - 1), 1.0f);
	
	for (uint32_t i = 0; i < mipCount; i++)
	{
		pushData.level = i ;
		pushData.roughness = i * deltaRoughness;
	
		s_Instance->m_queue->SetPushConstant(s_Instance->m_Specular.shader, PipelineBindPoint::Compute, ShaderStage::Compute,&pushData);
		s_Instance->m_queue->Dispatch(numWorkGroupsX, numWorkGroupsY, 6);
	}
	s_Instance->m_queue->FlushComputeQueue();

	env->specularMap->PrepareForRender();
	
	return env;
}

void Nexus::EnvironmentBuilder::ComputeWorker::Build(const std::string& shaderpath)
{
	ShaderSpecification shSpecs = ShaderCompiler::CompileFromFile(shaderpath);
	shader = GraphicsInterface::CreateShader(shSpecs);
	
	heap.set = 0;
	heap.hashId = UUID();
	shader->AllocateShaderResourceHeap(heap);

	ComputePipelineSpecification specs{};
	specs.shader = shader;
	pipeline = GraphicsInterface::CreatePipeline(specs);
}