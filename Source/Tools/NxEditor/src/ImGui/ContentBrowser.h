#pragma once
#include <filesystem>
#include "imgui.h"

#include "NxRenderEngine/ResourcePool.h"	

namespace NexusEd
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
		
		ImTextureID m_FileID, m_FolderID;
		Nexus::Ref<Nexus::Sampler> m_Sampler;

		bool m_enableImportOptions = false;
	};
}


