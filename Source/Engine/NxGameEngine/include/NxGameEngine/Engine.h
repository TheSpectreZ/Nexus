#pragma once

#include <cstdint>	
#include <vector>

#include "NxCore/KeyCode.h"

// Application
#include "Layer.h"
#include "NxCore/Window.h"

#include "NxRenderEngine/Renderer.h"

namespace Nexus
{
	struct GameEngineSpecifications
	{
		std::string cmdlineArgs;
		void* hInstance;

		const char* window_title;
		int window_width;
		int window_height;

		RenderEngineAPI eAPI;
	};

	class NEXUS_ENGINE_API GameEngine
	{
	public:
		GameEngine();
		~GameEngine();

		bool Init(const GameEngineSpecifications& specs);
		void Run();
		void Shut();

		template<typename T, typename... Args> requires std::derived_from<T, ILayer>
		T* AttachLayer(Args&&... args)
		{
			T* layer = new T(std::forward<Args>(args)...);
			mLayerStack.push_back(layer);
			return layer;
		}
	private:
		void OnScreenResize(int width, int height);
		void OnKeyPress(Key::Code key, InputAction action, InputModifier mods);
		void OnMousePress(Mouse::Code button, InputAction action, InputModifier mods);
	private:
		Window mWindow;
		RenderEngine mRenderEngine;
		std::vector< ILayer* > mLayerStack;
	};
}

