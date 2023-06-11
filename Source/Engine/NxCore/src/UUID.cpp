#include "NxCore/UUID.h"
#include <random>

namespace Nexus
{
	static std::random_device s_RandomDevice;
	static std::mt19937_64 eng(s_RandomDevice());
	static std::uniform_int_distribution<uint64_t> s_UniformDistribution;
}

Nexus::UUID::UUID(uint64_t Id)
{
	m_Id = Id;
}

Nexus::UUID::UUID(bool defaultValue)
{
	m_Id = defaultValue ? UINT64_MAX : s_UniformDistribution(eng);
}