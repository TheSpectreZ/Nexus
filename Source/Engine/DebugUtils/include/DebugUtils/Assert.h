#pragma once
#include "Build.h"

namespace Nexus
{
	namespace DebugUtils
	{
		void NEXUS_DEBUG_API Assert(bool Condition, const char* message,const char* file,int line);
	}
}

#ifdef NEXUS_DEBUG

#define NEXUS_ASSERT(cond,msg) Nexus::DebugUtils::Assert(cond,msg,__FILE__,__LINE__)

#else

#define NEXUS_ASSERT(cond,msg)

#endif // DEBUG
