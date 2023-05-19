#pragma once
#include <string>

namespace Nexus
{
	struct RenderSettings
	{
		bool EnableMultiSampling;
		bool EnableHDR;
	};

	struct ProjectSpecifications
	{
		std::string Version;
		std::string Name;
		std::string RootPath;

		RenderSettings renderSettings; 
	};
}