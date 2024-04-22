#pragma once
#include <filesystem>
#include "nlohmann/json.hpp"

#ifdef NEXUS_CORE_SHARED_BUILD
#define NEXUS_CORE_API __declspec(dllexport)
#else 
#define NEXUS_CORE_API __declspec(dllimport)
#endif // NEXUS_CORE_SHARED_BUILD

namespace Nexus
{
	typedef  std::filesystem::path AssetFilePath;


	enum class NEXUS_CORE_API AssetType
	{
		None, Mesh, Material, Texture
	};

	class NEXUS_CORE_API Asset
	{
	public:
		Asset(const AssetFilePath& path, AssetType type)
			:m_Filepath(path),m_Type(type)
		{}
		virtual ~Asset() = default;

		AssetFilePath GetFilepath() { return m_Filepath; }
		AssetType GetType() { return m_Type; }
		std::string GetTypeString();
	protected:
		AssetFilePath m_Filepath;
		AssetType m_Type;
	};

	class NEXUS_CORE_API Serializer
	{
	public:
		Serializer() = default;
		virtual ~Serializer() = default;
		
		virtual bool Serialize(const AssetFilePath& path) = 0;
		virtual bool Deserialize(const AssetFilePath& path) = 0;
	};

	class NEXUS_CORE_API BinarySerializer : public Serializer
	{
	public:
		BinarySerializer() = default;
		virtual ~BinarySerializer() = default;

		bool Serialize(const AssetFilePath& path) override;
		bool Deserialize(const AssetFilePath& path) override;
	protected:

		class NEXUS_CORE_API BinaryBlob
		{
		public:
			BinaryBlob() = default;
			~BinaryBlob() = default;
			
			void Resize(uint64_t size);

			void Write(void* data, uint64_t size);
			void Read(void* data, uint64_t size);

			uint64_t GetSize() { return m_data.size(); }
			void* GetMemory() { return m_data.data(); }
		private:
			std::vector<uint8_t> m_data;
			uint64_t m_offset = 0;
		};

		virtual BinaryBlob OnSerialize() = 0;
		virtual void OnDeserialize(BinaryBlob blob) = 0;
	};

	class NEXUS_CORE_API JsonSerializer : public Serializer
	{
	public:
		JsonSerializer() = default;
		virtual ~JsonSerializer() = default;

		bool Serialize(const AssetFilePath& path) override;
		bool Deserialize(const AssetFilePath& path) override;
	protected:
		virtual nlohmann::json OnSerialize() = 0;
		virtual void OnDeserialize(const nlohmann::json& Json) = 0;
	};

}