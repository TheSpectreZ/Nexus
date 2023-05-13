#pragma once
#include <filesystem>
#include "imgui.h"
#include "Renderer/Texture.h"

namespace Nexus
{
	class ContentBrowser
	{
	public:
		ContentBrowser() = default;
		~ContentBrowser() = default;

		void Initialize();
		void SetContext(const std::string& projectRootPath);
		void Render();
	private:
		std::filesystem::path m_AssetDirectory, m_CurrentDirectory;

		void DrawDirectoryNodes(std::filesystem::path path);
		void DrawDirectoryFiles(std::filesystem::path path);

		Ref<Texture> m_FileTexture, m_FolderTexture;
		ImTextureID m_FileID, m_FolderID;

		Ref<Sampler> m_Sampler;
	};
}


