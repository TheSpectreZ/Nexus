#pragma once
#include "NxCore/Base.h"

namespace Nexus
{
	enum class ImageType
	{
		Color, Depth, Resolve
	};

	enum class ImageOperation
	{
		Load, Store, Clear, DontCare
	};

	enum class ImageLayout
	{
		Undefined,
		ColorAttachment, DepthAttachment, DepthStencilAttachment,
		ShaderReadOnly, TransferSrc, TransferDst, PresentSrc
	};

	struct RenderpassAttachmentDescription
	{
		ImageType type;
		ImageOperation load;
		ImageOperation store;
		ImageLayout initialLayout;
		ImageLayout finalLayout;
		bool multiSampled;
		bool hdr;
	};

	struct SubpassDescription
	{
		std::vector<int32_t> ColorAttachments;
		std::vector<int32_t> InputAttachments;
		int32_t DepthAttachment = -1;
		int32_t ResolveAttachment = -1;
	};

	enum class PipelineStageFlag
	{
		None = 0,
		TopOfPipe = 1,
		BottomOfPipe = 2,
		VertexInput = 4,
		VertexShader = 8,
		FragmentShader = 16,
		EarlyFragmentTest = 32,
		LateFragmentTest = 64,
		ColorAttachmentOutput = 128,
		TransferBit = 256,
		ComputeBit = 512,
	};
	ENUM_FLAG_OPERATORS(PipelineStageFlag)

	enum class AccessFlag
	{
		None = 0,
		IndexRead = 1,
		UniformRead = 2,
		InputAttachmentRead = 4,
		ShaderRead = 8,
		ShaderWrite = 16,
		ColorAttachmentRead = 32,
		ColorAttachmentWrite = 64,
		DepthStencilAttachmentRead = 128,
		DepthStencilAttachmentWrite = 256,
		TransferRead = 512,
		TransferWrite = 1024,
	};
	ENUM_FLAG_OPERATORS(AccessFlag)

	struct SubpassDependency
	{
		static const uint32_t ExternalSubpass = ~0u;

		uint32_t srcSubpass, dstSubpass;
		PipelineStageFlag srcStageFlags, dstStageFlags;
		AccessFlag srcAccessFlags, dstAccessFlags;
	};

	struct RenderpassSpecification
	{
		std::vector<RenderpassAttachmentDescription>* attachments;
		std::vector<SubpassDescription>* subpasses;
		std::vector<SubpassDependency>* dependencies;
	};

	class Renderpass
	{
	public:
		Renderpass() = default;
		virtual ~Renderpass() = default;
	};
}