#pragma once
#include <string>
#include <unordered_map>

#include "NxCore/Base.h"

#include "TypeImpls.h"
#include "VertexInfo.h"
#include "Shader.h"

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
		//RenderTargetSpecification& set(Extent extent);
		RenderTargetSpecification& set(TextureFormat format);
		RenderTargetSpecification& set(RenderTargetUsage usage);
		RenderTargetSpecification& enableMSAA(bool multiSampled);

		//Extent extent = { 0,0 };
		TextureFormat format = TextureFormat::SWAPCHAIN_COLOR;
		RenderTargetUsage usage = RenderTargetUsage::ColorAttachment;
		bool multiSampled = false;
	};

	enum class RenderPipelineTopology
	{
		TriangleList, LineList
	};

	enum class RenderPipelinePolygonMode
	{
		Fill, Line
	};

	enum class RenderPipelineCullMode
	{
		Front, Back, Front_Back, None
	};

	enum class RenderPiplineFrontFaceType
	{
		Clockwise, AntiClockwise
	};

	struct NEXUS_GRAPHICS_API GraphicsRenderPipelineSpecification
	{
		GraphicsRenderPipelineSpecification& set(RenderPipelineTopology topology);
		GraphicsRenderPipelineSpecification& set(RenderPipelinePolygonMode polygon);
		GraphicsRenderPipelineSpecification& set(RenderPipelineCullMode cullMode);
		GraphicsRenderPipelineSpecification& set(RenderPiplineFrontFaceType face);
		GraphicsRenderPipelineSpecification& set(Ref<Shader> shader);
		
		GraphicsRenderPipelineSpecification& enableMSAA(bool multiSampled);

		VertexBindInfo& addVertexBinding();
		VertexAttribInfo& addVertexAttribute();

		RenderPipelineTopology topology;
		RenderPipelinePolygonMode polygon;
		RenderPipelineCullMode cull;
		RenderPiplineFrontFaceType frontface;

		std::vector<VertexBindInfo> bindInfo;
		std::vector<VertexAttribInfo> attribInfo;

		Ref<Shader> shader;
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

		RenderTargetSpecification& addOutput(const std::string& name);
		
		GraphicsRenderPipelineSpecification& addGraphicsPipeline(const std::string& name);
		
		RenderGraphPassSpecification& setRenderTargetExtent(Extent extent);

		const auto& getOutputs() const { return m_Outputs; }
		const auto& getGraphicsPipelines() const { return m_GraphicsPipelines; }

		Extent getRenderTargetExtent() const { return m_Extent; }
	protected:
		class RenderGraph* m_Parent = nullptr;
		
		std::vector<std::string> m_Outputs;
		std::vector<std::string> m_GraphicsPipelines;

		Extent m_Extent = { 0,0 };
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
		
		std::unordered_map<std::string, GraphicsRenderPipelineSpecification> m_GraphicsRenderPipelines;
	};
}
