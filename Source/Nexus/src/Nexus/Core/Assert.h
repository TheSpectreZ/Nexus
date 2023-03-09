#pragma once

namespace Nexus::Utility
{
	void Assert(bool condition, const char* message,const char* file,int line);
}

#ifdef NEXUS_DEBUG

#define NEXUS_ASSERT(cond,msg) Nexus::Utility::Assert(cond,msg,__FILE__,__LINE__)

#else

#define NEXUS_ASSERT(cond,msg)

#endif // DEBUG
