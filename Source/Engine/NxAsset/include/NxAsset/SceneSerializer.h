#pragma once
#include <string>
#include "NxScene/Scene.h"

#ifdef NEXUS_ASSET_SHARED_BUILD
#define NEXUS_ASSET_API __declspec(dllexport)
#else
#define NEXUS_ASSET_API __declspec(dllimport)
#endif // NEXUS_ASSET_SHARED_BUILD


namespace Nexus::Serializer
{
	bool NEXUS_ASSET_API SaveAsJson(Nexus::Ref<Scene> scene, const std::string& assetPath,const std::string& name);
}
