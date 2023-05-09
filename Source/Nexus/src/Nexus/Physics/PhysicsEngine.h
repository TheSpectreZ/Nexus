#pragma once

namespace Nexus
{
	enum class PhysicsAPIType
	{
		Jolt,None
	};
	
	class PhysicsEngine
	{
	public:
		static void Initialize(PhysicsAPIType api);
		static void Shutdown();

		PhysicsEngine() = default;
		virtual ~PhysicsEngine() = default;
	private:
		static Ref<PhysicsEngine> s_Instance;
		static PhysicsAPIType s_ApiType;

		friend class PhysicsWorld;
	};
}


