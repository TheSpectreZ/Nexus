#pragma once
#include "Build.h"
#include "Layer.h"

#include "Platform/Window.h"

#include <vector>

namespace Nexus
{
	class NEXUS_CORE_API Application
	{
	public:
		void Run();

		void PushLayer(Layer* layer);
		void PopLayer(Layer* layer);
	protected:
		Nexus::Platform::Window p_Window;
	private:
		std::vector<Layer*> m_layerstack;

		void PresenterCallback();
	};
}