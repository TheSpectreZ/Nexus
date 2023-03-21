#pragma once
#include "Vertex.h"
#include "Shader.h"

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
		TriangleList
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

	struct PipelineCreateInfo
	{
		Ref<Shader> shader;
		std::vector<VertexAttribInfo> vertexAttribInfo;
		std::vector<VertexBindInfo> vertexBindInfo;
		std::vector<PushConstantInfo> pushConstantInfo;

		RasterizerInfo rasterizerInfo;
	};

	class Pipeline
	{
	public:
		static Ref<Pipeline> Create(const PipelineCreateInfo& Info);

		Pipeline() = default;
		virtual ~Pipeline() = default;
	};
}
