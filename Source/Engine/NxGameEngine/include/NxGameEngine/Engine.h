#pragma once
#include "Layer.h"

#include "NxCore/KeyCode.h"
#include "NxCore/Window.h"
#include "NxCore/Event.h"

#include "NxRenderEngine/RenderEngine.h"

namespace Nexus
{
	struct GameEngineSpecifications
	{
		std::string cmdlineArgs;
		void* hInstance;

		const char* window_title;
		int window_width;
		int window_height;

		GraphicsAPI eAPI;
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
		void SetApplicationLayer(Args&&... args)
		{
			mApplicationLayer = std::make_unique<T>(std::forward<Args>(args)...);
			mApplicationLayer->mEngine = this;
		}
	private:
		void OnScreenResize(int width, int height);
		void OnKeyPress(Key::Code key, InputAction action, InputModifier mods);
		void OnMousePress(Mouse::Code button, InputAction action, InputModifier mods);
	public:
		RenderEngine* getRenderEngine() { return &mRenderEngine; }
	private:
		Window mWindow;
		RenderEngine mRenderEngine;
		
		friend class ILayer;
		std::unique_ptr<ILayer> mApplicationLayer;

		EventDispatcher mApplicationEventDispatcher;
	};
}

