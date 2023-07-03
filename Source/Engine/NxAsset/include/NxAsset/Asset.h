#pragma once
#include <filesystem>
#include <fstream>
#include "NxGraphics/AssetSpecifications.h"

#ifdef NEXUS_ASSET_SHARED_BUILD
#define NEXUS_ASSET_API __declspec(dllexport)
#else
#define NEXUS_ASSET_API __declspec(dllimport)
#endif // NEXUS_ASSET_SHARED_BUILD

namespace Nexus
{
	typedef std::filesystem::path AssetFilePath;

	namespace Utils
	{
		bool GetUUIDFromAssetFile(const AssetFilePath& path, UUID& Id, std::stringstream* stream);
	}

	enum class NEXUS_ASSET_API AssetType
	{
		None, Mesh, Texture
	};

	class NEXUS_ASSET_API Asset
	{
		friend class AssetManager;
	public:
		Asset(AssetType type) :m_Type(type) {}
		virtual ~Asset() = default;

		virtual bool Import(const AssetFilePath& Sourcefilepath, const AssetFilePath& AssetPath, const AssetFilePath& BinPath) { return false; };
		virtual bool Load(const AssetFilePath& AssetPath) { return false; }

		AssetType GetType() { return m_Type; }
	protected:
		AssetType m_Type;
	};

#define OVERRIDE_BASE_ASSET bool Import(const AssetFilePath& Sourcefilepath, const AssetFilePath& AssetPath, const AssetFilePath& BinPath) override; bool Load(const AssetFilePath& AssetPath) override;

	class NEXUS_ASSET_API MeshAsset : public Asset
	{
	public:
		MeshAsset() :Asset(AssetType::Mesh),m_Specs() {}
		~MeshAsset() override = default;

		OVERRIDE_BASE_ASSET

		const MeshSpecification& GetMeshSpecifications() { return m_Specs; }
	private:
		MeshSpecification m_Specs;
	};

	class NEXUS_ASSET_API TextureAsset : public Asset
	{
	public:
		TextureAsset() : Asset(AssetType::Texture),m_Specs() {}
		~TextureAsset() override = default;

		OVERRIDE_BASE_ASSET

		const TextureSpecification& GetTextureSpecifications() { return m_Specs; }
	private:
		TextureSpecification m_Specs;
	};
}