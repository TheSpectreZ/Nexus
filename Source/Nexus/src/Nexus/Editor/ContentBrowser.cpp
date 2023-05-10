#include "nxpch.h"
#include "ContentBrowser.h"

#include "imgui.h"

void Nexus::ContentBrowser::SetContext(const std::string& projectRootPath)
{
	m_AssetDirectory = std::string(projectRootPath + "\\Assets");
	m_CurrentDirectory = m_AssetDirectory;

	if (!std::filesystem::is_directory(m_AssetDirectory))
		std::filesystem::create_directory(m_AssetDirectory);
}

void Nexus::ContentBrowser::Render()
{
	ImGui::Begin("Content Browser");

	if (m_CurrentDirectory != std::filesystem::path(m_AssetDirectory))
	{
		if (ImGui::Button("<-"))
		{
			m_CurrentDirectory = m_CurrentDirectory.parent_path();
		}
	}

	static float padding = 24.f;
	static float thumbnailSize = 86.f;
	static float cellsize = thumbnailSize + padding;

	float panelWidth = ImGui::GetContentRegionAvail().x;

	int columnCount = (int)(panelWidth / (cellsize));
	columnCount = std::max(1, columnCount);
	ImGui::Columns(columnCount, 0, false);

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.1f, 0.1f, 1.f));
		
	int i = 0;
	for (auto& directory : std::filesystem::directory_iterator(m_CurrentDirectory))
	{
		ImGui::PushID(i++);

		const auto& path = directory.path();
		auto relativepath = std::filesystem::relative(path, m_AssetDirectory);

		std::string filenameString = relativepath.filename().string();

		if (directory.is_directory())
		{
			ImGui::Button(filenameString.c_str(), {thumbnailSize,thumbnailSize});

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				m_CurrentDirectory /= path.filename();
			}
		}
		else
		{
			ImGui::Button(filenameString.c_str(), { thumbnailSize,thumbnailSize });
		}

		//if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
		//{
		//	auto relativePath = std::filesystem::relative(path, m_AssetDirectory);
		//	const wchar_t* itemPath = relativePath.c_str();
		//	ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
		//	ImGui::EndDragDropSource();
		//}

		//ImGui::TextWrapped(filenameString.c_str());

		ImGui::NextColumn();
		ImGui::PopID();
	}
	ImGui::PopStyleColor();
	ImGui::Columns(1);

	ImGui::End();
}
