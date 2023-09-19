#pragma once
#include <string>
#include <unordered_map>

#include "NxCore/Base.h"
#include "TypeImpls.h"

#define RENDER_TARGET_BACKBUFFER_NAME "NexusBackBufferRT"

namespace Nexus
{
	enum class RenderTargetUsage
	{
		ColorAttachment,
		DepthAttachment,
		BackBuffer
	};

	struct NEXUS_GRAPHICS_API RenderTargetSpecification
	{
		RenderTargetSpecification& set(Extent extent);
		RenderTargetSpecification& set(TextureFormat format);
		RenderTargetSpecification& set(RenderTargetUsage usage);
		RenderTargetSpecification& enableMSAA(bool multiSampled);

		Extent extent = { 0,0 };
		TextureFormat format = TextureFormat::SWAPCHAIN_COLOR;
		RenderTargetUsage usage = RenderTargetUsage::ColorAttachment;
		bool multiSampled = false;
	};

	class NEXUS_GRAPHICS_API RenderGraphPassSpecification
	{
		friend class RenderGraph;
	public:
		RenderGraphPassSpecification() = default;
		~RenderGraphPassSpecification() = default;

		RenderGraphPassSpecification(class RenderGraph* pParent)
			:m_Parent(pParent) {}

		RenderTargetSpecification& setOutput(const std::string& name);

		const auto& getOutputs() const { return m_Outputs; }
	protected:
		class RenderGraph* m_Parent = nullptr;

		std::vector<std::string> m_Outputs;

		std::string m_ShaderPath;
	};

	class NEXUS_GRAPHICS_API RenderGraph
	{
		friend class RenderGraphPassSpecification;
	public:
		RenderGraph() = default;
		virtual ~RenderGraph() = default;

		virtual void Bake() = 0;
	
		RenderGraphPassSpecification& AddRenderGraphPass(const std::string& name);
	protected:
		std::unordered_map<std::string, RenderGraphPassSpecification> m_GPUpasses;
		std::unordered_map<std::string, RenderTargetSpecification> m_RenderTargets;
	};
}
