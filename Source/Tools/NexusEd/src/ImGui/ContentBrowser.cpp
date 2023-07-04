#include "ContentBrowser.h"
#include "Context.h"
#include "NxApplication/Application.h"
#include "NxAsset/Manager.h"

using namespace Nexus;

void NexusEd::ContentBrowser::Initialize()
{
	Nexus::SamplerSpecification samplerSpecs{};
	samplerSpecs.Far = Nexus::SamplerFilter::Linear;
	samplerSpecs.Near = Nexus::SamplerFilter::Linear;
	samplerSpecs.U = Nexus::SamplerWrapMode::Repeat;
	samplerSpecs.V = Nexus::SamplerWrapMode::Repeat;
	samplerSpecs.W = Nexus::SamplerWrapMode::Repeat;

	m_Sampler.reset();
	m_Sampler = Nexus::GraphicsInterface::CreateSampler(samplerSpecs);
	
	Module::AssetLoadResult file = Module::AssetManager::Get()->Load(AssetType::Texture, "Resources/Icons/File.NxAsset");
	Ref<Texture> filetexture = ResourcePool::Get()->AllocateTexture(DynamicPointerCast<TextureAsset>(file.asset)->GetTextureSpecifications(), file.id);
	m_FileID = Context::Get()->CreateTextureId(filetexture, m_Sampler);
	
	Module::AssetLoadResult folder = Module::AssetManager::Get()->Load(AssetType::Texture, "Resources/Icons/Folder.NxAsset");
	Ref<Texture> foldertexture = ResourcePool::Get()->AllocateTexture(DynamicPointerCast<TextureAsset>(folder.asset)->GetTextureSpecifications(), folder.id);
	m_FolderID = Context::Get()->CreateTextureId(foldertexture, m_Sampler);
}

void NexusEd::ContentBrowser::DrawDirectoryNodes(std::filesystem::path path)
{
	for (auto& dir : std::filesystem::directory_iterator(path))
	{
		if (!dir.is_directory())
			continue;

		const auto& p = dir.path();
		auto filenameString = p.filename().string();
		
		if (p == m_SelectedDirectory)
		{
			ImGui::SetNextItemOpen(true, ImGuiCond_Always);
			m_SelectedDirectory.clear();
		}

		if (ImGui::TreeNode(filenameString.c_str()))
		{
			m_CurrentDirectory = p;
			DrawDirectoryNodes(p);
			ImGui::TreePop();
		}
	}
}

void NexusEd::ContentBrowser::DrawDirectoryFiles(std::filesystem::path path)
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

		const auto& p = dir.path();
		auto filenameString = p.filename().string();

		if (dir.is_directory())
		{
			Context::Get()->BindTextureId(m_FolderID);
			if (ImGui::ImageButton(m_FolderID, { thumbnailSize,thumbnailSize }))
			{
				m_SelectedDirectory = p;
			}
		}
		else 
		{
			Context::Get()->BindTextureId(m_FileID);
			ImGui::ImageButton(m_FileID, { thumbnailSize,thumbnailSize });
			
			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
			{	
				const wchar_t* itemPath = p.c_str();
				ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
				ImGui::EndDragDropSource();
			}
		}
		ImGui::TextWrapped(filenameString.c_str());

		ImGui::PopID();
		ImGui::NextColumn();
	}
	ImGui::PopStyleColor();
	ImGui::Columns(1);
}

void NexusEd::ContentBrowser::SetContext(const std::string& projectRootPath)
{
	m_AssetDirectory = std::string(projectRootPath + "\\Assets"); 
	//[Note] Breaks when AssetDirectory is Root Directory for any Disk

	m_CurrentDirectory = m_AssetDirectory;

	if (!std::filesystem::is_directory(m_AssetDirectory))
		std::filesystem::create_directory(m_AssetDirectory);
}

void NexusEd::ContentBrowser::Render()
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

	auto w = Nexus::Application::Get()->GetWindow();
	auto m = ImGui::GetContentRegionMax();
	
	ImGui::End();
}