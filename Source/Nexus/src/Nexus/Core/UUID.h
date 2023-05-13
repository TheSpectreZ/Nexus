#pragma once
#include <cstdint>

namespace Nexus
{
	typedef uint64_t UUID;

	UUID CreateUUID();

	static UUID NullUUID = UINT64_MAX;
}
