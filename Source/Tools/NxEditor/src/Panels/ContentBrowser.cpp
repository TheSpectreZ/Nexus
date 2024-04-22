#include "ContentBrowser.h"
#include "imgui.h"

void Nexus::EditorPanel::ContentBrowser::Initialize(const std::filesystem::path& directoryPath)
{
	mRootPath = directoryPath;
}

void Nexus::EditorPanel::ContentBrowser::Destroy()
{
	mRootPath.clear();
}

void Nexus::EditorPanel::ContentBrowser::Render()
{
	ImGui::Begin("Content Browser");
	
	for (auto& dir : std::filesystem::directory_iterator(mRootPath))
	{
		auto& directoryPath = dir.path();
		auto name = directoryPath.stem().generic_string();
		
		ImGui::Button(name.c_str(), ImVec2(40, 20));
	}

	ImGui::End();
}
