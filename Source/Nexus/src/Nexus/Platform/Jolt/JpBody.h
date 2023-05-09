#pragma once
#include "Jolt/Jolt.h"
#include "Jolt/Physics/PhysicsScene.h"
#include "Jolt/Physics/Body/BodyInterface.h"

#include "Scene/Entity.h"

namespace Nexus
{
	struct JoltBody
	{
		JPH::Body* body;
		JPH::BodyID Id;
		JPH::EActivation activationflag;
	};

	class JoltBodyFactor
	{
	public:
		static JoltBody CreateRigidBody(JPH::BodyInterface& interface,Entity entity);
		static JoltBody CreateCollider(JPH::BodyInterface& interface, Entity entity);
	private:
		static JPH::Ref<JPH::Shape> BuildShape(Entity e);
	};
}

