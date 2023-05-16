#pragma once

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
		ColorAttachment, DepthAttachment,DepthStencilAttachment, 
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
		TopOfPipe,BottomOfPipe,VertexInput,VertexShader,
		FragmentShader,EarlyFragmentTest,LateFragmentTest,
		ColorAttachmentOutput,TransferBit,ComputeBit,
	};

	enum class AccessFlag
	{
		None,IndexRead,UniformRead,InputAttachmentRead,ShaderRead,ShaderWrite,
		ColorAttachmentRead,ColorAttachmentWrite,DepthStencilAttachmentRead,DepthStencilAttachmentWrite,
		TransferRead,TransferWrite,
	};

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
		static Ref<Renderpass> Create(const RenderpassSpecification& specs);
		Renderpass() = default;
		virtual ~Renderpass() = default;
	};
}
