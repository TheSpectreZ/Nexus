#pragma once

#ifdef NEXUS_NETWORK_ENGINE_SHARED_BUILD
#define NEXUS_NETWORK_ENGINE_API __declspec(dllexport)
#else
#define NEXUS_NETWORK_ENGINE_API __declspec(dllimport)
#endif

namespace Nexus
{
	class NEXUS_NETWORK_ENGINE_API NetworkEngine
	{
		static NetworkEngine* s_Instance;
	public:
		static NetworkEngine* Get() { return s_Instance; }

		static void Initialize();
		static void Shutdown();
	};
}