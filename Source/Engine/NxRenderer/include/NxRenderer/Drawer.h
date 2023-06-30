#pragma once
#include "NxScene/Scene.h"
#include "GraphicsInterface.h"

#include "RenderableMesh.h"	

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

		virtual void Draw(Ref<Scene> scene) = 0;
		virtual void OnWindowResize(Extent extent) = 0;
	};

	class NEXUS_RENDERER_API ForwardDrawer : public Drawer
	{
	public:
		ForwardDrawer();
		~ForwardDrawer() override = default;

		void Draw(Ref<Scene> scene) override;
		void OnWindowResize(Extent extent) override;
	private:
		FramebufferSpecification m_fbSpecs;

		Viewport m_Viewport;
		Scissor m_Scissor;

		Ref<Renderpass> m_pass;
		Ref<Framebuffer> m_fb;

		Ref<Shader> m_shader;
		Ref<Pipeline> m_pipeline;

		Ref<RenderableMesh> m_Mesh;
		ResourceHeapHandle m_Heap;
		UniformBufferHandle m_Uniform;
	};
}
