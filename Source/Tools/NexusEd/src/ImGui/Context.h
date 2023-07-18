#pragma once
#include "NxCore/Base.h"
#include "NxGraphics/TypeImpls.h"
#include "NxGraphics/Texture.h"
#include "imgui.h"

namespace NexusEd
{
	class Context
	{
		friend class Viewport;
		friend class ContentBrowser;
		friend class SceneHeirarchy;
		static Context* s_Instance;
	public:
		static Context* Get() { return s_Instance; }
		static void Initialize();
		static void Shutdown();
		static void OnWindowResize(Nexus::Extent extent);

		void BeginFrame();
		void EndFrame();
		
		ImTextureID CreateTextureId(Nexus::Ref<Nexus::Texture> texture, Nexus::Ref<Nexus::Sampler> sampler);
		void DestroyTextureId(ImTextureID Id);
		void BindTextureId(ImTextureID Id);
	private:
		void ImplFonts();
		void ImplStyles();

		void ImplWindowInit();
		void ImplWindowShut();

		void ImplVulkanInit();
		void ImplVulkanShut();

		uint32_t GetFrameIndex();
	};
}
