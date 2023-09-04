#include "NxCore/Base.h"
#include <sstream>
#include <codecvt>
const std::string WHITESPACE = " \n\r\t\f\v";

std::string ltrim(const std::string& s)
{
	size_t start = s.find_first_not_of(WHITESPACE);
	return (start == std::string::npos) ? "" : s.substr(start);
}

std::string rtrim(const std::string& s)
{
	size_t end = s.find_last_not_of(WHITESPACE);
	return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

std::string trim(const std::string& s) {
	return rtrim(ltrim(s));
}

std::string Nexus::BuildString(const std::wstring& wStr)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
	return myconv.to_bytes(wStr);
}

std::unordered_map<std::string, std::string> Nexus::BuildCCMap(const std::string& args)
{
	std::vector<std::string> items;
	{
		std::stringstream ss(args);
		std::string line;

		while (std::getline(ss, line, ' '))
			items.push_back(line);
	}

	std::unordered_map<std::string, std::string> map;
	for(auto& arg : items)
	{
		auto trimmed = trim(arg);

		if (trimmed.length() > 1 && trimmed[0] == '-')
		{
			auto curEquals = trimmed.find_first_of('=');

			if (curEquals == std::string::npos)
			{
				auto curKey = trimmed.substr(1);
				map[curKey] = "true";
			}
			else if (curEquals != std::string::npos && curEquals > 1)
			{
				auto curKey = trimmed.substr(1, curEquals - 1);
				auto curValue = trimmed.substr(curEquals + 1);
				map[curKey] = curValue;
			}
		}
	}

	return map;
}
