#pragma once
#include <string>

#ifdef NEXUS_CORE_SHARED_BUILD
#define NEXUS_CORE_API __declspec(dllexport)
#else 
#define NEXUS_CORE_API __declspec(dllimport)
#endif // NEXUS_CORE_SHARED_BUILD

namespace Nexus
{
	std::string NEXUS_CORE_API wstring_to_string(const std::wstring& wString);
	std::string NEXUS_CORE_API wstring_to_string(const wchar_t* wString);
	
	std::wstring NEXUS_CORE_API string_to_wstring(const std::string& String);
	std::wstring NEXUS_CORE_API string_to_wstring(const char* String);
}
