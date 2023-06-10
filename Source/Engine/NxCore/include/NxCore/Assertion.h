#pragma once
#include "Logger.h"

#define NEXUS_BREAK_ASSERT(falseCondition,log,...) if(falseCondition)\
{\
	NEXUS_LOG("Assert",log,__VA_ARGS__);\
	__debugbreak();\
}

#define NEXUS_ASSERT(falseCondition,log,...) if(falseCondition)\
{\
	NEXUS_LOG("Assert",log,__VA_ARGS__);\
}
