#pragma once
#include <filesystem>

namespace Nexus::EditorPanel
{
	class ContentBrowser
	{
	public:
		void Initialize(const std::filesystem::path& directoryPath);
		void Destroy();

		void Render();
	private:
		std::filesystem::path mRootPath;
	};
}
