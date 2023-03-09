#pragma once

namespace Nexus
{
	class RenderCommandQueue
	{
	public:
		static Ref<RenderCommandQueue> Create();

		RenderCommandQueue() = default;
		virtual ~RenderCommandQueue() = default;

		virtual void Begin() = 0;
		virtual void End() = 0;
		virtual void Flush() = 0;
	};
}