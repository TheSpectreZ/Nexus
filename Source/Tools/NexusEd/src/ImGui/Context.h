#pragma once
#include "NxGraphics/TypeImpls.h"
#include "imgui.h"

namespace NexusEd
{
	class Context
	{
		friend class Viewport;
		static Context* s_Instance;
	public:
		static Context* Get() { return s_Instance; }
		static void Initialize();
		static void Shutdown();
		static void OnWindowResize(Nexus::Extent extent);

		void BeginFrame();
		void EndFrame();
	private:
		void ImplFonts();
		void ImplStyles();

		void ImplWindowInit();
		void ImplWindowShut();

		void ImplVulkanInit();
		void ImplVulkanShut();

		void BindTextureId(ImTextureID Id);
		uint32_t GetFrameIndex();
	};
}
