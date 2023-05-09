#pragma once
#include "Physics/PhysicsWorld.h"

#include "JpImpls.h"
#include "Jolt/Physics/PhysicsSystem.h"

namespace Nexus
{
	class JoltPhysicsWorld : public PhysicsWorld
	{
		static const uint32_t cMaxBodies = 1024;
		static const uint32_t cMaxBodyPairs = 1024;
		static const uint32_t cMaxContactConstraints = 1024;
		static const uint32_t cNumBodyMutexes = 0;
	public:
		JoltPhysicsWorld();
		~JoltPhysicsWorld();

		void OnSceneStart(Ref<Scene> Scene) override;
		void OnSceneUpdate(float dt) override;
		void OnSceneStop() override;
	private:
		JPH::PhysicsSystem* m_PhysicsSystem;
		JPH::BodyInterface* m_BodyInterface;

		std::unordered_map<uint32_t, Entity> m_ActiveEntities;
		std::vector<JPH::BodyID> m_Bodies;

		static ObjectLayerPairFilterImpl m_ObjLayerPairFilter;
		static BPLayerInterfaceImpl m_BpLayerInterface;
		static ObjectVsBroadPhaseLayerFilterImpl m_ObjVsBpLayerFilter;
	};
}
