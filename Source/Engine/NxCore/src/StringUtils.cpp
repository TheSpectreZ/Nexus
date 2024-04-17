#include "NxCore/StringUtils.h"
#include <codecvt>
#include <cstdarg>

static std::wstring_convert<std::codecvt_utf8<wchar_t>> sConverter;

std::string Nexus::wstring_to_string(const std::wstring& wString)
{
	return sConverter.to_bytes(wString);
}

std::string Nexus::wstring_to_string(const wchar_t* wString)
{
	return sConverter.to_bytes(wString);
}

std::wstring Nexus::string_to_wstring(const std::string& String)
{
	return sConverter.from_bytes(String);
}

std::wstring Nexus::string_to_wstring(const char* String)
{
	return sConverter.from_bytes(String);
}