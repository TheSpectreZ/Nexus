#include "nxpch.h"
#include "JpEngine.h"

#include "Jolt/RegisterTypes.h"
#include "Jolt/Core/Factory.h"
#include "Jolt/Physics/PhysicsSettings.h"

static void TraceImpl(const char* inFMT, ...)
{
	NEXUS_LOG_TRACE("JOLT: {0}", inFMT);
}

static bool AssertImpl(const char* inExp, const char* inMessage, const char* inFile, uint32_t inLine)
{
	std::string msg = fmt::format("[ {} ]", inExp);
	Nexus::Utility::Assert(true, msg.c_str(), inFile, inLine);
	return true;
}

Nexus::JoltPhysicsEngine* Nexus::JoltPhysicsEngine::sInstance = nullptr;

Nexus::JoltPhysicsEngine::JoltPhysicsEngine()
{
	sInstance = this;

	JPH::RegisterDefaultAllocator();
	JPH::Trace = TraceImpl;
	JPH_IF_ENABLE_ASSERTS(JPH::AssertFailed = AssertImpl);

	JPH::Factory::sInstance = new JPH::Factory();
	JPH::RegisterTypes();

	m_Allocator = new JPH::TempAllocatorImpl(10 * 1024 * 1024);
	m_JobSystem = new JPH::JobSystemThreadPool(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, std::thread::hardware_concurrency() - 1);
	
	NEXUS_LOG_WARN("Jolt Physics Engine Initialized");
}

Nexus::JoltPhysicsEngine::~JoltPhysicsEngine()
{
	JPH::UnregisterTypes();
	
	delete m_Allocator;
	delete m_JobSystem;

	delete JPH::Factory::sInstance;
	JPH::Factory::sInstance = nullptr;

	NEXUS_LOG_WARN("Jolt Physics Engine Shutdown");
}
