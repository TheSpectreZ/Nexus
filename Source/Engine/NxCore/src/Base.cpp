#include "NxCore/Base.h"
#include <codecvt>

std::wstring Nexus::ToWString(const std::string& str)
{
	std::wstring_convert< std::codecvt_utf8<wchar_t> > c;
	return c.from_bytes(str);
}
std::string Nexus::FromWString(const std::wstring& wStr)
{
	std::wstring_convert< std::codecvt_utf8<wchar_t> > c;
	return c.to_bytes(wStr);
}