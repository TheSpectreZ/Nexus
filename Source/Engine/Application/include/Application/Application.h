#pragma once
#include "Build.h"
#include "Layer.h"

#include "Platform/Window.h"

#include <vector>
#include <memory>

namespace Nexus
{
	class NEXUS_CORE_API Application
	{
	public:
		Application();
		void Run();

		template<typename T>
		void PushLayer();
	protected:
		Nexus::Platform::Window p_Window;
	private:
		std::vector<std::shared_ptr<Layer>> m_layerstack;

		void OnWindowResize(uint32_t width, uint32_t height);
	};

	template<typename T>
	inline void Application::PushLayer()
	{
		static_assert(std::is_base_of<Layer, T>::value, "Pushed type is not subclass of Layer!");
		m_layerstack.push_back(std::make_shared<T>());
	}

}