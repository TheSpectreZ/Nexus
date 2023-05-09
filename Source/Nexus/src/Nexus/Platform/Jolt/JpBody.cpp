#include "nxpch.h"
#include "JpBody.h"
#include "JpImpls.h"

#include "Jolt/Physics/Collision/Shape/BoxShape.h"

namespace JoltUtils
{
	JPH::Vec3 ToJoltVector3(glm::vec3 vector)
	{
		return { vector.x,vector.y,vector.z };
	}

	JPH::Quat ToJoltQuat(glm::quat quat)
	{
		return { quat.x,quat.y,quat.z,quat.w };
	}

	JPH::EMotionType ToJoltMotionType(Nexus::Component::RigidBody::MotionType Type)
	{
		switch (Type)
		{
		case Nexus::Component::RigidBody::MotionType::Dynamic:
			return JPH::EMotionType::Dynamic;
		case Nexus::Component::RigidBody::MotionType::Kinematic:
			return JPH::EMotionType::Kinematic;
		default:
			return JPH::EMotionType::Static;
		}
	}
}

Nexus::JoltBody Nexus::JoltBodyFactor::CreateRigidBody(JPH::BodyInterface& interface, Entity entity)
{
	auto& Transform = entity.GetComponent<Component::Transform>();
	auto& RigidBody = entity.GetComponent<Component::RigidBody>();

	JPH::Ref<JPH::Shape> shape = BuildShape(entity);
	NEXUS_ASSERT((shape == nullptr), "Jolt Shape Building Failed");

	JPH::BodyCreationSettings setting(shape.GetPtr(),
		JoltUtils::ToJoltVector3(Transform.Translation),
		JoltUtils::ToJoltQuat(Transform.GetRotation()),
		JoltUtils::ToJoltMotionType(RigidBody.motionType),
		(RigidBody.motionType == Component::RigidBody::MotionType::Static) ? Layers::NON_MOVING : Layers::MOVING);

	setting.mMassPropertiesOverride.mMass = RigidBody.mass;
	setting.mRestitution = 0.4f;

	JoltBody JB;
	JB.body = interface.CreateBody(setting);
	JB.Id = JB.body->GetID();

	if (RigidBody.Simulate)
		JB.activationflag = JPH::EActivation::Activate;
	else
		JB.activationflag = JPH::EActivation::DontActivate;

	return JB;
}

Nexus::JoltBody Nexus::JoltBodyFactor::CreateCollider(JPH::BodyInterface& interface, Entity entity)
{
	auto& Transform = entity.GetComponent<Component::Transform>();
	
	JPH::Ref<JPH::Shape> shape = BuildShape(entity);
	NEXUS_ASSERT((shape == nullptr), "Jolt Shape Building Failed");
	
	JPH::BodyCreationSettings setting(shape.GetPtr(),
		JoltUtils::ToJoltVector3(Transform.Translation),
		JoltUtils::ToJoltQuat(Transform.GetRotation()),
		JPH::EMotionType::Static, Layers::NON_MOVING);

	JoltBody JB;
	JB.body = interface.CreateBody(setting);
	JB.Id = JB.body->GetID();
	JB.activationflag = JPH::EActivation::Activate;
	
	return JB;
}

JPH::Ref<JPH::Shape> Nexus::JoltBodyFactor::BuildShape(Entity e)
{
	if (e.HasComponent<Component::BoxCollider>())
	{
		auto& collider = e.GetComponent<Component::BoxCollider>();

		JPH::BoxShapeSettings boxShapeSettings = JPH::BoxShapeSettings(JoltUtils::ToJoltVector3(collider.HalfExtent));
		JPH::ShapeSettings::ShapeResult boxShapeResult = boxShapeSettings.Create();

		return boxShapeResult.Get();
	}

	NEXUS_ASSERT(true, "Rigid Body Creation Failed due to Lack of Collider Component");
	return nullptr;
}