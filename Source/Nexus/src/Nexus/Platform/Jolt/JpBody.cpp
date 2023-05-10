#include "nxpch.h"
#include "JpBody.h"
#include "JpImpls.h"

#include "Jolt/Physics/Collision/Shape/BoxShape.h"
#include "Jolt/Physics/Collision/Shape/SphereShape.h"
#include "Jolt/Physics/Collision/Shape/CapsuleShape.h"
#include "Jolt/Physics/Collision/Shape/CylinderShape.h"
#include "Jolt/Physics/Collision/Shape/TaperedCapsuleShape.h"

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

Nexus::JoltBody Nexus::JoltBodyFactor::CreateRigidBody(JPH::BodyInterface& interface, Entity entity)
{
	auto& Transform = entity.GetComponent<Component::Transform>();
	auto& RigidBody = entity.GetComponent<Component::RigidBody>();

	JPH::Ref<JPH::Shape> shape = BuildShape(entity);
	NEXUS_ASSERT((shape == nullptr), "Jolt Shape Building Failed");

	JPH::BodyCreationSettings setting(shape.GetPtr(),
		JoltUtils::ToJoltVector3(Transform.Translation),
		JoltUtils::ToJoltQuat(Transform.GetRotation()),
		ToJoltMotionType(RigidBody.motionType),
		(RigidBody.motionType == Component::RigidBody::MotionType::Static) ? Layers::NON_MOVING : Layers::MOVING);

	setting.mMassPropertiesOverride.mMass = RigidBody.mass;
	setting.mRestitution = RigidBody.restitution;
	setting.mFriction = RigidBody.friction;

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
	else if (e.HasComponent<Component::SphereCollider>())
	{
		auto& collider = e.GetComponent<Component::SphereCollider>();

		JPH::SphereShapeSettings sphereShapeSettings = JPH::SphereShapeSettings(collider.Radius);
		JPH::ShapeSettings::ShapeResult result = sphereShapeSettings.Create();

		return result.Get();
	}
	else if (e.HasComponent<Component::CapsuleCollider>())
	{
		auto& collider = e.GetComponent<Component::CapsuleCollider>();

		if (collider.TopRadius == collider.TopRadius)
		{
			JPH::CapsuleShapeSettings settings = JPH::CapsuleShapeSettings(collider.HalfHeight, collider.TopRadius);
			JPH::ShapeSettings::ShapeResult result = settings.Create();

			return result.Get();
		}
		else
		{
			JPH::TaperedCapsuleShapeSettings settings = JPH::TaperedCapsuleShapeSettings(collider.HalfHeight, collider.TopRadius,collider.BottomRadius);
			JPH::ShapeSettings::ShapeResult result = settings.Create();

			return result.Get();
		}
	}
	else if (e.HasComponent<Component::CylinderCollider>())
	{
		auto& collider = e.GetComponent<Component::CylinderCollider>();

		JPH::CylinderShapeSettings settings = JPH::CylinderShapeSettings(collider.HalfHeight, collider.Radius);
		JPH::ShapeSettings::ShapeResult result = settings.Create();

		return result.Get();
	}

	NEXUS_ASSERT(true, "Rigid Body Creation Failed due to Lack of Collider Component");
	return nullptr;
}