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
		virtual bool OnWindowResize(int width, int height) { return false; }
		virtual bool OnKeyPress(Key::Code key, InputAction action, InputModifier mods) { return false; }
		virtual bool OnMousePress(Mouse::Code key, InputAction action, InputModifier mods) { return false; }
	};

	class ILayer : public ILayerEvents
	{
	public:
		virtual ~ILayer() {}
		virtual void Create() {}
		virtual void Update(float dt) {}
		virtual void Destroy() {}
	};
}
