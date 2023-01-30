#pragma once
#include "Core/Layer.h"

#include "Graphics/Framebuffer.h"
#include "Graphics/Renderpass.h"
#include "Graphics/Pipeline.h"
#include "Graphics/Descriptor.h"
#include "Graphics/Resource.h"

#include "Graphics/Camera.h"

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
	Nexus::Graphics::GraphicsPipeline pipeline;

	// Clear Value
	std::vector<VkClearValue> clearValue;

	// Camera
	Nexus::Graphics::Camera cam;
	Nexus::Graphics::CameraController Controller;

	// Uniform
	Nexus::Graphics::UniformBuffer ubuffer;

	// Mesh
	Nexus::Graphics::VertexBuffer vbuffer;
	Nexus::Graphics::IndexBuffer ibuffer;

	// Screen
	VkViewport viewport;
	VkRect2D scissor;
};

