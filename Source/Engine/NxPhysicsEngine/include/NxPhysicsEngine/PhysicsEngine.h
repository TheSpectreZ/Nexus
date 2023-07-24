#pragma once
#include "NxScene/Scene.h"

#if NEXUS_PHYSICS_ENGINE_SHARED_BUILD
#define NEXUS_PHYSICS_ENGINE_API __declspec(dllexport)
#else
#define NEXUS_PHYSICS_ENGINE_API __declspec(dllimport)
#endif // NEXUS_PHYSICS_ENGINE_SHARED_BUILD

namespace Nexus
{
	class NEXUS_PHYSICS_ENGINE_API PhysicsEngine
	{
		static PhysicsEngine* s_Instance;
	public:
		static PhysicsEngine* Get() { return s_Instance; }
		static void Initialize();
		static void Shutdown();

		static void OnSceneStart(Ref<Scene> scene);
		static void OnSceneUpdate(float dt);
		static void OnSceneStop();
	private:
		void CreateRigidbody(Entity e);
	};
}