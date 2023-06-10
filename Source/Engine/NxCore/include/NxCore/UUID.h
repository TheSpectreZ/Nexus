#pragma once
#include <cstdint>

#ifdef NEXUS_CORE_SHARED_BUILD
#define NEXUS_CORE_API __declspec(dllexport)
#else 
#define NEXUS_CORE_API __declspec(dllimport)
#endif // NEXUS_CORE_SHARED_BUILD

namespace Nexus
{
	class NEXUS_CORE_API UUID
	{
	public:
		UUID(bool defaultValue = false);

		operator uint64_t() { return m_Id; }
		operator bool() { return m_Id != UINT64_MAX; }

		bool operator ==(const UUID& other) { return m_Id == other.m_Id; }
		bool operator !=(const UUID& other) { return m_Id != other.m_Id; }
	private:
		uint64_t m_Id;
	};
}
