#include "nxpch.h"
#include "PhysicsWorld.h"

#include "PhysicsEngine.h"
#include "Platform/Jolt/JpWorld.h"

Nexus::Ref<Nexus::PhysicsWorld> Nexus::PhysicsWorld::Create()
{
	switch (PhysicsEngine::s_ApiType)
	{
	case Nexus::PhysicsAPIType::Jolt: return CreateRef<JoltPhysicsWorld>();
	case Nexus::PhysicsAPIType::None: return nullptr;
	default: return nullptr;
	}
}
