#pragma once
#include "RenderTypes.h"
#include "Pipeline.h"

#include "Framebuffer.h"
#include "Mesh.h"
#include "Material.h"

namespace Nexus
{
	class Command
	{
	public:
		static Ref<Command> GetRef();

		static void Init();
		static void Shut();

		static void Update();

		static void TransferStaticMesh(Ref<StaticMesh> mesh);
		static void TransferMaterial(Ref<Material> material);

		static void SetViewport(const Viewport& viewport);
		static void SetScissor(const Scissor& scissor);
		
		static void BeginRenderpass(Ref<Renderpass> renderpass, Ref<Framebuffer> framebuffer);
		static void EndRenderpass();

		static void BindPipeline(Ref<Pipeline> pipeline);
		static void DrawMesh(Ref<StaticMesh> mesh);
	private:
		virtual void ImplInit() = 0;
		virtual void ImplUpdate() = 0;

		virtual void ImplBeginRenderpass(Ref<Renderpass> r, Ref<Framebuffer> f) = 0;
		virtual void ImplEndRenderpass() = 0;
		virtual void ImplTransferStaticMesh(Ref<StaticMesh> mesh) = 0;
		virtual void ImplTransferMaterial(Ref<Material> material) = 0;
		virtual void ImplBindPipeline(Ref<Pipeline> pipeline) = 0;
		virtual void ImplDrawMesh(Ref<StaticMesh> mesh) = 0;
		virtual void ImplSetViewport(const Viewport& viewport) = 0;
		virtual void ImplSetScissor(const Scissor& scissor) = 0;
	};
}