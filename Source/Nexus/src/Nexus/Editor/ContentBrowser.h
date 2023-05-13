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
		std::filesystem::path m_AssetDirectory, m_CurrentDirectory, m_SelectedDirectory;
		
		void DrawDirectoryNodes(std::filesystem::path path);
		void DrawDirectoryFiles(std::filesystem::path path);
		void DrawDirectoryFilesTopBar();

		Ref<Texture> m_FileTexture, m_FolderTexture, m_ForwardTexture, m_BackwardTexture;
		ImTextureID m_FileID, m_FolderID, m_ForwardID, m_BackwardID;

		Ref<Sampler> m_Sampler;
	};
}


