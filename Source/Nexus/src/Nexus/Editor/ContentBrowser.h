#pragma once
#include <filesystem>

namespace Nexus
{
	class ContentBrowser
	{
	public:
		void SetContext(const std::string& projectRootPath);
		void Render();
	private:
		std::filesystem::path m_AssetDirectory, m_CurrentDirectory;

		void DrawDirectoryNodes(std::filesystem::path path);
		void DrawDirectoryFiles(std::filesystem::path path);
	};
}


