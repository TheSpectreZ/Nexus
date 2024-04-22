#pragma once

#ifdef NEXUS_ASSET_SHARED_BUILD
#define NEXUS_ASSET_API __declspec(dllexport)
#else
#define NEXUS_ASSET_API __declspec(dllimport)
#endif // NEXUS_ASSET_SHARED_BUILD

namespace Nexus
{
	class NEXUS_ASSET_API Asset
	{

	};
}