#include "nxpch.h"
#include "PhysicsEngine.h"

#include "Platform/Jolt/JpEngine.h"

Nexus::Ref<Nexus::PhysicsEngine> Nexus::PhysicsEngine::s_Instance = nullptr;
Nexus::PhysicsAPIType Nexus::PhysicsEngine::s_ApiType = Nexus::PhysicsAPIType::None;

void Nexus::PhysicsEngine::Initialize(PhysicsAPIType api)
{
	s_ApiType = api;
	switch (api)
	{
	case Nexus::PhysicsAPIType::Jolt:
		s_Instance = CreateRef<JoltPhysicsEngine>();
		break;
	case Nexus::PhysicsAPIType::None:
		s_Instance = nullptr;
		break;
	default:
		s_Instance = nullptr;
		break;
	}
}

void Nexus::PhysicsEngine::Shutdown()
{
	s_Instance.reset();
}
