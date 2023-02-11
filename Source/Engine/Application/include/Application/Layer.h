#pragma once
#include "Build.h"

typedef unsigned int uint32_t;

namespace Nexus
{
	class Layer
	{
	public:
		virtual void OnAttach() = 0;
		virtual void OnDetach() = 0;
		
		virtual void OnUpdate() = 0;
		virtual void OnRender() = 0;

		virtual void OnWindowResize(uint32_t width, uint32_t height) = 0;
	};
}