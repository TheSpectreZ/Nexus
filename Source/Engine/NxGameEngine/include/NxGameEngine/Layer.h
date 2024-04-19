#pragma once
#include "NxCore/KeyCode.h"

#ifdef NEXUS_ENGINE_SHARED_BUILD
#define NEXUS_ENGINE_API __declspec(dllexport)
#else 
#define NEXUS_ENGINE_API __declspec(dllimport)
#endif // NEXUS_CORE_SHARED_BUILD

namespace Nexus
{
	class ILayerEvents
	{
	public:
		virtual ~ILayerEvents() {}
		virtual void OnWindowResize(int width, int height) {}
		virtual void OnKeyPress(Key::Code key, InputAction action, InputModifier mods) {}
		virtual void OnMousePress(Mouse::Code key, InputAction action, InputModifier mods) {}
	};

	class NEXUS_ENGINE_API ILayer : public ILayerEvents
	{
	public:
		virtual ~ILayer() {}
		virtual void Create() {}
		virtual void Update(float dt) {}
		virtual void Render() {}
		virtual void Destroy() {}
	protected:
		class GraphicsDevice* getGraphicsDevice();
	private:
		friend class GameEngine;
		class GameEngine* mEngine = nullptr;
	};
}
