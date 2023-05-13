#include "nxpch.h"
#include "UUID.h"
#include <random>

namespace Nexus
{
	static std::random_device s_RandomDevice;
	static std::mt19937_64 eng(s_RandomDevice());
	static std::uniform_int_distribution<uint64_t> s_UniformDistribution;
}

Nexus::UUID Nexus::CreateUUID()
{
	return s_UniformDistribution(eng);
}