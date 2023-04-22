#pragma once
#include "RenderTypes.h"
#include "Pipeline.h"

#include "Mesh.h"

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

		static void SetViewport(const Viewport& viewport);
		static void SetScissor(const Scissor& scissor);
		
		static void BindPipeline(Ref<Pipeline> pipeline);
		static void DrawMesh(Ref<StaticMesh> mesh);
	private:
		virtual void ImplInit() = 0;
		virtual void ImplUpdate() = 0;

		virtual void ImplTransferStaticMesh(Ref<StaticMesh> mesh) = 0;
		virtual void ImplBindPipeline(Ref<Pipeline> pipeline) = 0;
		virtual void ImplDrawMesh(Ref<StaticMesh> mesh) = 0;
		virtual void ImplSetViewport(const Viewport& viewport) = 0;
		virtual void ImplSetScissor(const Scissor& scissor) = 0;
	};
}