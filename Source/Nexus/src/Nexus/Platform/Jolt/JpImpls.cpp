#include "nxpch.h"
#include "JpImpls.h"


JPH::Vec3 Nexus::JoltUtils::ToJoltVector3(const glm::vec3& vector)
{
	return { vector.x,vector.y,vector.z };
}

JPH::Quat Nexus::JoltUtils::ToJoltQuat(const glm::quat& quat)
{
	return { quat.x,quat.y,quat.z,quat.w };
}

bool Nexus::ObjectLayerPairFilterImpl::ShouldCollide(JPH::ObjectLayer o1, JPH::ObjectLayer o2) const
{
	switch (o1)
	{
	case Layers::NON_MOVING:
		return o2 == Layers::MOVING;
	case Layers::MOVING:
		return true;
	default:
		NEXUS_ASSERT(false, "ObjectLayerPairFilterImpl Error");
		return false;
	}
}

Nexus::BPLayerInterfaceImpl::BPLayerInterfaceImpl()
{
	mObjectToBroadPhase[Layers::NON_MOVING] = BroadPhaseLayers::NON_MOVING;
	mObjectToBroadPhase[Layers::MOVING] = BroadPhaseLayers::MOVING;
}

uint32_t Nexus::BPLayerInterfaceImpl::GetNumBroadPhaseLayers() const
{
	return BroadPhaseLayers::NUM_LAYERS;
}

JPH::BroadPhaseLayer Nexus::BPLayerInterfaceImpl::GetBroadPhaseLayer(JPH::ObjectLayer layer) const
{
	NEXUS_ASSERT(!(layer < Layers::NUM_LAYERS), "BroadPhaseLayerInterfaceImpl Error");
	return mObjectToBroadPhase[layer];
}

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
	const char* Nexus::BPLayerInterfaceImpl::GetBroadPhaseLayerName(JPH::BroadPhaseLayer inLayer) const
	{
		switch ((JPH::BroadPhaseLayer::Type)inLayer)
		{
		case (JPH::BroadPhaseLayer::Type)BroadPhaseLayers::NON_MOVING:
			return "NON_MOVING";
		case (JPH::BroadPhaseLayer::Type)BroadPhaseLayers::MOVING:
			return "MOVING";
		default:
			NEXUS_ASSERT(true, "BroadPhaseLayerInterfaceImpl Error");
			return "<Invalid>";
		}
	}
#endif

bool Nexus::ObjectVsBroadPhaseLayerFilterImpl::ShouldCollide(JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2) const
{
	switch (inLayer1)
	{
	case Layers::NON_MOVING:
		return inLayer2 == BroadPhaseLayers::MOVING;
	case Layers::MOVING:
		return true;
	default:
		NEXUS_ASSERT(true, "ObjectVsBroadPhaseLayerFilterImpl Error");
		return false;
	}
}