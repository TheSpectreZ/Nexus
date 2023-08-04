#pragma once
#include "TypeImpls.h"
#include "Renderpass.h"
#include "Shader.h"
#include "ShaderResource.h"
#include "VertexInfo.h"

namespace Nexus
{
	enum class PolygonMode
	{
		Fill, Line
	};

	enum class CullMode
	{
		Front, Back, None
	};

	enum class FrontFaceType
	{
		Clockwise, AntiClockwise
	};

	enum class TopologyType
	{
		TriangleList, LineList
	};

	struct RasterizerInfo
	{
		PolygonMode polygonMode;
		CullMode cullMode;
		FrontFaceType frontFace;
		TopologyType topology;
		float lineWidth;
	};

	struct PushConstantInfo
	{
		ShaderStage stage;
		uint32_t offset, size;
	};

	struct GraphicsPipelineSpecification
	{
		Ref<Shader> shader;
		Ref<Renderpass> renderpass;
		uint32_t subpass;
		bool multisampled;
		std::vector<VertexAttribInfo> vertexAttribInfo;
		std::vector<VertexBindInfo> vertexBindInfo;
		RasterizerInfo rasterizerInfo;
	};

	struct ComputePipelineSpecification
	{
		Ref<Shader> shader;
	};

	enum class PipelineBindPoint { Graphics, Compute };

	class Pipeline
	{
	public:
		Pipeline() = default;
		virtual ~Pipeline() = default;
	};
}