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

	enum class RenderPipelineVertexInputRate
	{
		PerVertex, PerInstance
	};

	enum class RenderPipelineVertexAttribFormat
	{
		Vec2, Vec3, Vec4
	};

	struct NEXUS_GRAPHICS_API GraphicsRenderPipelineSpecification
	{
		GraphicsRenderPipelineSpecification& set(Ref<Shader> shader);
		GraphicsRenderPipelineSpecification& set(RenderPipelineTopology topology);
		GraphicsRenderPipelineSpecification& set(RenderPipelinePolygonMode polygon);
		GraphicsRenderPipelineSpecification& set(RenderPipelineCullMode cullMode);
		GraphicsRenderPipelineSpecification& set(RenderPiplineFrontFaceType face);
		GraphicsRenderPipelineSpecification& set(RenderPipelineVertexInputRate rate, uint32_t stride);
		
		GraphicsRenderPipelineSpecification& addVertexAttrib(RenderPipelineVertexAttribFormat format, uint32_t location , uint32_t offset);
		
		GraphicsRenderPipelineSpecification& enableMSAA(bool multiSampled);

		RenderPipelineTopology topology;
		RenderPipelinePolygonMode polygon;
		RenderPipelineCullMode cull;
		RenderPiplineFrontFaceType frontface;

		uint32_t vertexStride = 0;
		RenderPipelineVertexInputRate vertexInputRate;

		std::vector< std::tuple<RenderPipelineVertexAttribFormat, uint32_t, uint32_t> > attribInfo;

		Ref<Shader> shader;
		bool multiSampled = false;
		
	};

	class NEXUS_GRAPHICS_API RenderGraphPassSpecification
	{
		friend class RenderGraph;
	public:
		RenderGraphPassSpecification() = default;
		~RenderGraphPassSpecification() = default;

		RenderGraphPassSpecification(class RenderGraph* pParent,const std::string& name)
			:m_Parent(pParent),m_Name(name) {}

		RenderTargetSpecification& addOutput(const std::string& name);
		const auto& getOutputs() const { return m_Outputs; }
		
		GraphicsRenderPipelineSpecification& addGraphicsPipeline(const std::string& name);
		const auto& getGraphicsPipelines() const { return m_GraphicsPipelines; }

		RenderGraphPassSpecification& setRenderTargetExtent(Extent extent);
		Extent getRenderTargetExtent() const { return m_Extent; }

		RenderGraphPassSpecification& addGraphDependency(const std::string& name);
		RenderGraphPassSpecification& promoteToBackBuffer();
	protected:
		class RenderGraph* m_Parent = nullptr;
		std::string m_Name;

		std::vector<std::string> m_Outputs;
		std::vector<std::string> m_GraphicsPipelines;

		std::string m_PassParent;
		std::vector<std::string> m_PassDependency;

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

		std::string m_backBuffer;
	};
}
