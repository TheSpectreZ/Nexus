#pragma once

#ifdef NEXUS_CORE_SHARED_BUILD
#define NEXUS_CORE_API __declspec(dllexport)
#else 
#define NEXUS_CORE_API __declspec(dllimport)
#endif // NEXUS_CORE_SHARED_BUILD

namespace Nexus::Utility
{
	void NEXUS_CORE_API Assert(bool condition, const char* message,const char* file,int line);
}

#ifdef NEXUS_DEBUG

#define NEXUS_ASSERT(cond,msg) Nexus::Utility::Assert(cond,msg,__FILE__,__LINE__)

#else

#define NEXUS_ASSERT(cond,msg)

#endif // DEBUG
