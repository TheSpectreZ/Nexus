#pragma once
#include "imgui.h"

/// 
/// Currently Supports Vulkan Only - Will Make it More Abstract When Other APIs are Added
/// 

namespace Nexus
{

	class EditorContext
	{
	public:
		static void Initialize();
		static void Shutdown();

		static void StartFrame();
		static void Render();
	};

}