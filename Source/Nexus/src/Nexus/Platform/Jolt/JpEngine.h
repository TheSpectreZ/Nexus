#pragma once
#include "Physics/PhysicsEngine.h"

#include "Jolt/Jolt.h"
#include "Jolt/Core/TempAllocator.h"
#include "Jolt/Core/JobSystemThreadPool.h"

namespace Nexus
{
	class JoltPhysicsEngine : public PhysicsEngine
	{
		friend class JoltPhysicsWorld;
		static JoltPhysicsEngine* sInstance;
	public:
		JoltPhysicsEngine();
		~JoltPhysicsEngine() override;
	private:
		JPH::TempAllocatorImpl* m_Allocator;
		JPH::JobSystemThreadPool* m_JobSystem;	
	};
}


