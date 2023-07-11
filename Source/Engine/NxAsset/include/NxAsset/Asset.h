#pragma once
#include <filesystem>
#include <fstream>
#include "NxCore/UUID.h"
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
		None, Mesh, Texture, MaterialTable
	};

	class NEXUS_ASSET_API Asset
	{
		friend class AssetManager;
	public:
		Asset(AssetType type) :m_Type(type), m_Name("Empty-Asset"),m_Id(UUID(true)) {}
		virtual ~Asset() = default;

		virtual bool Load(const AssetFilePath& AssetPath) { return false; }

		AssetType GetType() { return m_Type; }
		std::string GetName() { return m_Name; }
		UUID GetID() { return m_Id; }
	protected:
		UUID m_Id;
		AssetType m_Type;
		std::string m_Name;
	};

	// [To-Do] Think About using UUID as Bin File name for uniqueness

#define OVERRIDE_BASE_ASSET static bool Import(const AssetFilePath& Sourcefilepath, const AssetFilePath& AssetPath, const AssetFilePath& BinPath,const std::string& AssetName); bool Load(const AssetFilePath& AssetPath) override;

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

	class NEXUS_ASSET_API MaterialTableAsset : public Asset
	{
	public:
		MaterialTableAsset() : Asset(AssetType::MaterialTable) {}
		~MaterialTableAsset() override = default;

		OVERRIDE_BASE_ASSET

		const MaterialTableSpecification GetMaterialTableSpecifications() { return m_Specs; }
	private:
		MaterialTableSpecification m_Specs;
	};
}