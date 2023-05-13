#pragma once
#include "imgui.h"
#include "Renderer/FrameBuffer.h"
#include "Renderer/ShaderResource.h"
#include "Renderer/Texture.h"

namespace Nexus
{
	class EditorContext
	{
		friend class EditorViewport;
	public:
		EditorContext() = default;
		virtual ~EditorContext() = default;

		static void Initialize(Ref<Renderpass> renderpass);
		static void Shutdown();

		static void StartFrame();
		static void Render();
	private:
		virtual void Start() = 0;
		virtual void End() = 0;
		
		virtual ImTextureID MakeTextureID(Ref<Framebuffer> framebuffer, Ref<Sampler> sampler, uint32_t bufferIndex, uint32_t imageIndex) = 0;
		virtual ImTextureID MakeTextureID(Ref<Texture> texture, Ref<Sampler> sampler) = 0;
		virtual void DestroyTextureID(ImTextureID Id) = 0;
		
		
		virtual void BindTextureID(ImTextureID Id) = 0;

		static EditorContext* s_Instance;
	};
}