#pragma once
#include <cstdint>
#include <vector>
#include <array>
#include <variant>

namespace Nexus
{
	struct Viewport
	{
		float x, y;
		float width, height;
		float minDepth, maxDepth;
	};

	struct Offset
	{
		int x, y;
	};

	struct Extent
	{
		unsigned int width, height;
	};

	struct Scissor
	{
		Offset Offset;
		Extent Extent;
	};

	enum class GPUBufferType
	{
		eVertex, 
		eIndex,
		eUniform,
		eStorage
	};

	enum class ImageFormat
	{
		eUndefined,
		eSwapchainColor,

		eRGBA8_SFLOAT,
		eD32_SFLOAT
	};

	enum class ImageLoadOp
	{
		eClear,
		eLoad,
		eDontCare
	};

	enum class ImageStoreOp
	{
		eStore,
		eDontCare
	};

	enum class ImageLayout
	{
		eColorAttachment,
		eDepthStencilAttachment,
		eTransferSrc,
		eTransferDst,
	};
	
	struct ImageClearColorValue
	{
		ImageClearColorValue(float r, float g, float b, float a) { rgba = { r,g,b,a }; }

		std::array<float, 4> rgba = { 0.f, 0.f, 0.f, 1.f };
	};

	struct ImageClearDepthStencilValue
	{
		ImageClearDepthStencilValue(float d, uint32_t s) { depth = d; stencil = s; }

		float depth = 0.f; uint32_t stencil = 0;
	};

	typedef std::variant<ImageClearColorValue, ImageClearDepthStencilValue> ImageClearValue;
}

#define ENUM_TO_STRING(EnumValue) #EnumValue