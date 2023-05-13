#include "nxpch.h"
#include "EditorContext.h"
#include "ContentBrowser.h"

void Nexus::ContentBrowser::Initialize()
{
	m_Sampler = Sampler::Create(SamplerFilter::Linear, SamplerFilter::Linear);

	m_FileTexture = Texture::LoadFromFile("Resources/Icons/File.png");
	m_FolderTexture = Texture::LoadFromFile("Resources/Icons/Folder.png");

	m_FileID = EditorContext::s_Instance->MakeTextureID(m_FileTexture, m_Sampler);
	m_FolderID = EditorContext::s_Instance->MakeTextureID(m_FolderTexture, m_Sampler);
}

void Nexus::ContentBrowser::DrawDirectoryNodes(std::filesystem::path path)
{
	for (auto& dir : std::filesystem::directory_iterator(path))
	{
		if (!dir.is_directory())
			continue;

		const auto& p = dir.path();
		auto filenameString = p.filename().string();

		if (ImGui::TreeNode(filenameString.c_str()))
		{
			m_CurrentDirectory = p;
			DrawDirectoryNodes(p);
			ImGui::TreePop();
		}
	}
}

void Nexus::ContentBrowser::DrawDirectoryFiles(std::filesystem::path path)
{
	static float padding = 24.f;
	static float thumbnailSize = 86.f;
	static float cellsize = thumbnailSize + padding;

	float panelWidth = ImGui::GetContentRegionAvail().x;

	int columnCount = (int)(panelWidth / (cellsize));
	columnCount = std::max(1, columnCount);

	ImGui::Columns(columnCount, 0, false);

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));

	int i = 0;
	for (auto& dir : std::filesystem::directory_iterator(path))
	{
		ImGui::PushID(i++);

		const auto& path = dir.path();
		auto filenameString = path.filename().string();

		if (dir.is_directory())
		{
			EditorContext::s_Instance->BindTextureID(m_FolderID);
			ImGui::ImageButton(m_FolderID, {100.f,100.f});
		}
		else 
		{
			EditorContext::s_Instance->BindTextureID(m_FileID);
			ImGui::ImageButton(m_FileID, {100.f,100.f});
		}

		ImGui::PopID();
		ImGui::NextColumn();
	}
	ImGui::PopStyleColor();
	ImGui::Columns(1);
}

void Nexus::ContentBrowser::SetContext(const std::string& projectRootPath)
{
	m_AssetDirectory = std::string(projectRootPath + "\\Assets"); 
	//[Note] Breaks when AssetDirectory is Root Directory for any Disk

	m_CurrentDirectory = m_AssetDirectory;

	if (!std::filesystem::is_directory(m_AssetDirectory))
		std::filesystem::create_directory(m_AssetDirectory);
}

void Nexus::ContentBrowser::Render()
{
	ImGui::Begin("Content Browser");

	if (ImGui::BeginTable("Content", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Resizable))
	{
		ImGui::TableSetupColumn("Outline", 0, 250.f);
		ImGui::TableSetupColumn("Files", ImGuiTableColumnFlags_WidthStretch);

		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);

		ImGui::BeginChild("Folders");
		if (ImGui::TreeNode(m_AssetDirectory.filename().string().c_str()))
		{
			m_CurrentDirectory = m_AssetDirectory;
			DrawDirectoryNodes(m_AssetDirectory);
			ImGui::TreePop();
		}
		ImGui::EndChild();

		ImGui::TableSetColumnIndex(1);

		ImGui::BeginChild("Files", { ImGui::GetContentRegionAvail().x, ImGui::GetWindowSize().y });
		DrawDirectoryFiles(m_CurrentDirectory);
		ImGui::EndChild();

		ImGui::EndTable();
	}

	ImGui::End();
}
