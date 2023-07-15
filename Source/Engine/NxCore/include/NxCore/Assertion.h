#pragma once
#include "Logger.h"

#define NEXUS_BREAK_ASSERT(falseCondition,log,...) if(falseCondition)\
{\
	NEXUS_LOG("Assert",log,__VA_ARGS__);\
	__debugbreak();\
}

#ifndef NEXUS_DEBUG

#define NEXUS_ASSERT(falseCondition,log,...) if(falseCondition)\
{\
	NEXUS_LOG("Assert",log,__VA_ARGS__);\
}

#else 

#define NEXUS_ASSERT(falseCondition,log,...) NEXUS_BREAK_ASSERT(falseCondition,log,__VA_ARGS__)

#endif // NEXUS_DEBUG