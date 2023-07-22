#pragma once
#include "NxCore/Window.h"
#include "NxGraphics/TypeImpls.h"	
#include "NxGraphics/Texture.h"
#include "NxRenderEngine/RendererAPI.h"

#include "imgui.h"

#ifdef NEXUS_IMGUI_SHARED_BUILD
#define NEXUS_IMGUI_API __declspec(dllexport)
#else
#define NEXUS_IMGUI_API __declspec(dllimport)
#endif // NEXUS_IMGUI_SHARED_BUILD


namespace NxImGui
{
	class NEXUS_IMGUI_API Context
	{
	public:
		static void Initialize(const Nexus::Window& window,Nexus::RendererAPI api);
		static void Shutdown();

		static void StartFrame();
		static void EndFrame();

		static void OnWindowResize(const Nexus::Extent& extent);

		static ImTextureID CreateTextureID(Nexus::Ref<Nexus::Texture> texture, Nexus::Ref<Nexus::Sampler> sampler);
		static ImTextureID CreateTextureID(void* vkImageview, Nexus::Ref<Nexus::Sampler> sampler);
		
		static void BindTextureID(ImTextureID Id);
		static void DestroyTextureID(ImTextureID Id);

		static uint32_t GetFrameIndex();
	};
}