#pragma once
#include <filesystem>
#include "NxGraphics/Renderables.h"

#ifdef NEXUS_ASSET_SHARED_BUILD
#define NEXUS_ASSET_API __declspec(dllexport)
#else
#define NEXUS_ASSET_API __declspec(dllimport)
#endif // NEXUS_ASSET_SHARED_BUILD

namespace Nexus
{
	enum class NEXUS_ASSET_API AssetType
	{
		None, Mesh, Texture
	};

	class NEXUS_ASSET_API Asset
	{
		friend class AssetManager;
	public:
		Asset()
			:m_Type(AssetType::None),m_Path("")
		{}
		virtual ~Asset() = default;

		virtual bool Import(const std::filesystem::path& Sourcefilepath, const std::filesystem::path& AssetPath, const std::filesystem::path& BinPath) { return false; };
		virtual bool Load(const std::filesystem::path& AssetPath) { return false; }

		AssetType GetType() { return m_Type; }
		std::string GetPath() { return m_Path; }
	protected:
		AssetType m_Type;
		std::string m_Path;
	};

	class NEXUS_ASSET_API MeshAsset : public Asset
	{
	public:
		MeshAsset() = default;
		~MeshAsset() override = default;

		bool Import(const std::filesystem::path& Sourcefilepath, const std::filesystem::path& AssetPath, const std::filesystem::path& BinPath);
		bool Load(const std::filesystem::path& AssetPath);

		const MeshSpecifications& GetMeshSpecifications() { return m_Specs; }
	private:
		MeshSpecifications m_Specs;
	};
}