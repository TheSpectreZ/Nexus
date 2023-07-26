#pragma once
#include "NxScene/Scene.h"
#include "GraphicsInterface.h"

#include "RenderableScene.h"

#ifdef NEXUS_RENDERER_SHARED_BUILD
#define NEXUS_RENDERER_API __declspec(dllexport)
#else
#define NEXUS_RENDERER_API __declspec(dllimport)
#endif // NEXUS_RENDERER_SHARED_BUILD

namespace Nexus
{
	class NEXUS_RENDERER_API Drawer
	{
	public:
		Drawer() = default;
		virtual ~Drawer() = default;

		virtual void OnWindowResize(Extent extent) = 0;
	};

	class NEXUS_RENDERER_API ForwardDrawer : public Drawer
	{
	public:
		ForwardDrawer(bool renderToTexture = false);
		~ForwardDrawer() override = default;

		void Draw(Ref<Scene> scene);
		void OnWindowResize(Extent extent) override;

		Ref<Framebuffer> GetFramebuffer() { return m_fb; }
		uint32_t GetResolveIndex() { return 2; }
	private:
		FramebufferSpecification m_fbSpecs;

		Viewport m_Viewport;
		Scissor m_Scissor;

		Ref<Renderpass> m_pass;
		Ref<Framebuffer> m_fb;

		Ref<Shader> m_PbrShader,m_SkyboxShader;
		Ref<Pipeline> m_PBR_FillPipeline, m_PBR_LinePipeline, m_SkyboxPipeline;

		//std::unordered_map<UUID, Ref<RenderableScene>> m_RenderableScenes;
	};
}
