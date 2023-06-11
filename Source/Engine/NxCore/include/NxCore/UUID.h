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
		UUID(uint64_t Id);
		UUID(bool defaultValue = false);

		operator uint64_t() const { return m_Id; }
		operator bool() const { return m_Id != UINT64_MAX; }
		
		operator uint64_t() { return m_Id; }
		operator bool() { return m_Id != UINT64_MAX; }

		void operator =(const uint64_t& Id) { m_Id = Id; }
		bool operator ==(const UUID& other) const { return m_Id == other.m_Id; }
		bool operator !=(const UUID& other) { return m_Id != other.m_Id; }
	private:
		uint64_t m_Id;
	};
}

namespace std
{
	template<typename T> struct hash;

	template<>
	struct hash<Nexus::UUID>
	{
		std::uint64_t operator()(const Nexus::UUID& Id) const
		{
			return Id;
		}
	};
}
