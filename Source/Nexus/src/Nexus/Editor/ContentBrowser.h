#pragma once
#include <filesystem>
#include "imgui.h"

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

		ImTextureID m_FileID, m_FolderID, m_ForwardID, m_BackwardID;
	};
}


