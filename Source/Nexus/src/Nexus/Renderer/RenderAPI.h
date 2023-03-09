#pragma once

namespace Nexus
{

	enum class RenderAPIType
	{
		NONE, VULKAN
	};

	class RenderAPI
	{
		static RenderAPIType s_Type;
	public:
		static void SetRenderAPI(RenderAPIType Type);
		static RenderAPIType GetCurrentAPI();
	};

}