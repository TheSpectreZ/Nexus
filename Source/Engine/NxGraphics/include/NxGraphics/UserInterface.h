#pragma once
#include "Framebuffer.h"

namespace Nexus
{
	class UserInterface
	{
	public:
		virtual ~UserInterface() = default;
		virtual void Begin() = 0;
		virtual void End() = 0;

		virtual void OnWindowResize(int width, int height) = 0;

		virtual std::pair<Framebuffer*, uint32_t> getPresentable() = 0;
	};
}