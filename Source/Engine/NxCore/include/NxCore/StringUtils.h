#pragma once
#include <string>
#include <memory>
#include "Assertion.h"

#ifdef NEXUS_CORE_SHARED_BUILD
#define NEXUS_CORE_API __declspec(dllexport)
#else 
#define NEXUS_CORE_API __declspec(dllimport)
#endif // NEXUS_CORE_SHARED_BUILD

namespace Nexus
{
	NEXUS_CORE_API std::string wstring_to_string(const std::wstring& wString);
	NEXUS_CORE_API std::string wstring_to_string(const wchar_t* wString);
	
	NEXUS_CORE_API std::wstring string_to_wstring(const std::string& String);
	NEXUS_CORE_API std::wstring string_to_wstring(const char* String);

	template<typename ... Args>
	inline std::string format_string(const std::string& format, Args ... args)
	{
		size_t size = snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
	
		NEXUS_ASSERT(size <= 0, "Error during Formatting");
		
		std::unique_ptr<char[]> buf(new char[size]);
		snprintf(buf.get(), size, format.c_str(), args ...);
		
		return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
	}
}
