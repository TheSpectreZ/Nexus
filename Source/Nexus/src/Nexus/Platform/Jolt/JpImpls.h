#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include "glm/gtx/quaternion.hpp"

#include "Jolt/Jolt.h"
#include "Jolt/Physics/Collision/ObjectLayer.h"
#include "Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h"

using namespace JPH::literals;

namespace Nexus
{
	namespace JoltUtils
	{
		JPH::Vec3 ToJoltVector3(const glm::vec3& vector);
		JPH::Quat ToJoltQuat(const glm::quat& quat);
	}

	namespace Layers
	{
		static constexpr JPH::ObjectLayer NON_MOVING = 0;
		static constexpr JPH::ObjectLayer MOVING = 1;
		static constexpr JPH::ObjectLayer NUM_LAYERS = 2;
	}

	class ObjectLayerPairFilterImpl :public JPH::ObjectLayerPairFilter
	{
	public:
		bool ShouldCollide(JPH::ObjectLayer o1, JPH::ObjectLayer o2) const override;
	};

	namespace BroadPhaseLayers
	{
		static constexpr JPH::BroadPhaseLayer NON_MOVING(0);
		static constexpr JPH::BroadPhaseLayer MOVING(1);
		static constexpr uint32_t NUM_LAYERS = 2;
	}

	class BPLayerInterfaceImpl : public JPH::BroadPhaseLayerInterface
	{
	public:
		BPLayerInterfaceImpl();
		uint32_t GetNumBroadPhaseLayers() const override;
		JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer layer) const override;
		
#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
		const char* GetBroadPhaseLayerName(JPH::BroadPhaseLayer inLayer) const override;
#endif
	private:
		JPH::BroadPhaseLayer mObjectToBroadPhase[Layers::NUM_LAYERS];
	};

	class ObjectVsBroadPhaseLayerFilterImpl : public JPH::ObjectVsBroadPhaseLayerFilter
	{
	public:
		bool ShouldCollide(JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2) const override;
	};

}