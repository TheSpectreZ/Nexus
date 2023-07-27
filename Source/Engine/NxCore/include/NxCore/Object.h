#pragma once
#include <filesystem>

#ifdef NEXUS_CORE_SHARED_BUILD
#define NEXUS_CORE_API __declspec(dllexport)
#else 
#define NEXUS_CORE_API __declspec(dllimport)
#endif // NEXUS_CORE_SHARED_BUILD

namespace Nexus
{
	typedef std::filesystem::path AssetFilePath;

	class BaseAsset
	{
	public:
		BaseAsset() = default;
		virtual ~BaseAsset() = default;

		virtual std::string GetAssetTypeString() = 0;
		virtual uint8_t GetAssetTypeIndex() = 0;

		AssetFilePath GetFilepath() { return m_Filepath; }
	protected:
		AssetFilePath m_Filepath;
	};
}