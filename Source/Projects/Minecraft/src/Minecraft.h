#pragma once
#include "Core/Layer.h"

#include "Graphics/Framebuffer.h"
#include "Graphics/Renderpass.h"
#include "Graphics/Pipeline.h"
#include "Graphics/Descriptor.h"
#include "Graphics/Resource.h"

#include "Graphics/Camera.h"

#include "Game/World.h"

#include <unordered_map>

class Minecraft : public Nexus::Layer
{
public:
	void OnAttach() override;
	void OnUpdate() override;
	void OnRender() override;
	void OnDetach() override;
	void OnCallback() override;
private:
	void CreateAttachments();
	void CreateFramebuffers();
	void CreateRenderpasses();
	void CreatePipelines();
	void CreateDescriptors();

	void UpdateCamera();
private:
	// Attachments
	std::vector<Nexus::Graphics::FramebufferAttachment> ColorAttachments;
	Nexus::Graphics::FramebufferAttachment DepthAttachment;

	// Framebuffers
	std::vector<Nexus::Graphics::Framebuffer> Framebuffers;

	// Renderpass
	Nexus::Graphics::Renderpass renderpass;
	
	// Descriptors
	Nexus::Graphics::DescriptorPool descriptorPool;
	Nexus::Graphics::DescriptorLayout descriptorLayout;
	Nexus::Graphics::DescriptorSet descriptorSet;

	// Pipelines 
	Nexus::Graphics::PipelineLayout pipelineLayout;
	
	std::unordered_map<uint32_t, Nexus::Graphics::GraphicsPipeline> pipelines;

	// Clear Value
	std::vector<VkClearValue> clearValue;

	struct InstanceData
	{
		glm::vec3 cameraPos; float null;
	};

	// Instance
	InstanceData instancedata;

	// Uniform
	Nexus::Graphics::UniformBuffer worldbuffer;
	Nexus::Graphics::UniformBuffer instancebuffer;

	// Texture
	Nexus::Graphics::Texture2D texture;
	// Sampler
	Nexus::Graphics::Sampler sampler;

	// Screen
	VkViewport viewport;
	VkRect2D scissor;

	// World
	World m_world;
};

