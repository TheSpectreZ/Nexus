#pragma once
#include <string>

#ifdef NEXUS_CORE_SHARED_BUILD
#define NEXUS_CORE_API __declspec(dllexport)
#else
#define NEXUS_CORE_API __declspec(dllimport)
#endif // NEXUS_CORE_SHARED_BUILD


namespace Nexus
{
	struct NEXUS_CORE_API RenderSettings
	{
		bool EnableMultiSampling;
		bool EnableHDR;
	};

	struct NEXUS_CORE_API ProjectSpecifications
	{
		std::string Version;
		std::string Name;
		std::string RootPath;

		RenderSettings renderSettings; 
	};
}