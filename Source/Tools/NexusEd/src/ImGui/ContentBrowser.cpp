#include "ContentBrowser.h"
#include "Context.h"
#include "NxCore/Logger.h"
#include "NxApplication/FileDialog.h"
#include "NxAsset/Asset.h"

#include "NxGraphics/Meshing.h"

using namespace Nexus;

void NexusEd::ContentBrowser::Initialize()
{
	Nexus::SamplerSpecification samplerSpecs{};
	samplerSpecs.sampler.Far = Nexus::SamplerFilter::Linear;
	samplerSpecs.sampler.Near = Nexus::SamplerFilter::Linear;
	samplerSpecs.sampler.U = Nexus::SamplerWrapMode::Repeat;
	samplerSpecs.sampler.V = Nexus::SamplerWrapMode::Repeat;
	samplerSpecs.sampler.W = Nexus::SamplerWrapMode::Repeat;

	m_Sampler = Nexus::ResourcePool::Get()->GetSampler(samplerSpecs);

	{
		Meshing::Image fileImage;
		auto[res,id] = Importer::LoadImage("Resources/Icons/File.NxTex", fileImage);

		TextureSpecification specs{ fileImage };

		Ref<Texture> Texture = ResourcePool::Get()->AllocateTexture(specs, id);
		m_FileID = Context::Get()->CreateTextureId(Texture, m_Sampler);
	}
	
	{		
		Meshing::Image folderImage;
		auto [res, id] = Importer::LoadImage("Resources/Icons/Folder.NxTex", folderImage);
		
		TextureSpecification specs{ folderImage };

		Ref<Texture> Texture = ResourcePool::Get()->AllocateTexture(specs, id);
		m_FolderID = Context::Get()->CreateTextureId(Texture, m_Sampler);
	}
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
		
		if (ImGui::TreeNodeEx(filenameString.c_str(),ImGuiTreeNodeFlags_Framed))
		{
			m_CurrentDirectory = p;
			DrawDirectoryNodes(p);
			ImGui::TreePop();
		}
	}
}

void NexusEd::ContentBrowser::SetContext(const std::string& projectRootPath)
{
	m_AssetDirectory = std::filesystem::current_path() / "Resources";
	m_BinDirectory = m_AssetDirectory / "Bin";
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

		ImGui::SetNextItemOpen(true, ImGuiCond_FirstUseEver);
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.f);
		if (ImGui::TreeNodeEx(m_AssetDirectory.filename().string().c_str(), ImGuiTreeNodeFlags_Framed))
		{
			m_CurrentDirectory = m_AssetDirectory;
			DrawDirectoryNodes(m_AssetDirectory);
			ImGui::TreePop();
		}
		ImGui::PopStyleVar();

		ImGui::EndChild();

		ImGui::TableSetColumnIndex(1);

		ImGui::BeginChild("Files", { ImGui::GetContentRegionAvail().x, ImGui::GetWindowSize().y });
		DrawDirectoryFiles(m_CurrentDirectory);
		ImGui::EndChild();

		ImGui::EndTable();
	}

	ImGui::End();
	if (m_enableImportOptions)
		DrawImportOptions();
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

	Context::Get()->BindTextureId(m_FolderID);
	Context::Get()->BindTextureId(m_FileID);
	
	int i = 0;
	for (auto& dir : std::filesystem::directory_iterator(path))
	{
		ImGui::PushID(i++);

		const auto& p = dir.path();
		auto filenameString = p.filename().string();

		static int Renaming = -1;

		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 4.f);
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.f);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 8.f,4.f });

		if (dir.is_directory())
		{
			ImGui::ImageButton(filenameString.c_str(), m_FolderID, { thumbnailSize,thumbnailSize });

			if(ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				m_SelectedDirectory = p;
			}
		}
		else
		{
			ImGui::ImageButton(filenameString.c_str(), m_FileID, {thumbnailSize,thumbnailSize});
			
			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
			{
				const wchar_t* itemPath = p.c_str();
				ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
				ImGui::EndDragDropSource();
			}
		}

		if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right, false))
			ImGui::OpenPopup("FileOptions");

		if (Renaming != i)
			ImGui::TextWrapped(filenameString.c_str());
		else
		{
			static std::string input;
			if (!input.size())
				input.resize(256);

			ImGui::SetKeyboardFocusHere();
			if (ImGui::InputText("##FileNameInput", &input[0], input.size() + 1, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				Renaming = -1;

				AssetFilePath newPath = p.parent_path() / input;
				input.clear();

				std::filesystem::rename(p, newPath);
			}
		}

		ImGui::PopStyleVar(3);

		static bool showDeleteModal = false;
		static bool dontShowModal = false;

		if (ImGui::BeginPopup("FileOptions"))
		{
			if (ImGui::MenuItem("Rename"))
				Renaming = i;

			if (ImGui::MenuItem("Delete"))
				showDeleteModal = true;

			ImGui::EndPopup();
		}

		if (showDeleteModal)
		{
			if (!dontShowModal)
				ImGui::OpenPopup("Delete ?");
			else
				std::filesystem::remove_all(p);

			showDeleteModal = false;

			ImVec2 center = ImGui::GetMainViewport()->GetCenter();
			ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		}

		if (ImGui::BeginPopupModal("Delete ?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Are you sure to delete this?");
			
			if (ImGui::Button("Delete", ImVec2(85.f, 25.f)))
			{
				std::filesystem::remove_all(p);
				ImGui::CloseCurrentPopup();
			}
			
			ImGui::SameLine();
			
			if (ImGui::Button("Cancel", ImVec2(85.f, 25.f)))
				ImGui::CloseCurrentPopup();
			
			ImGui::Checkbox("Don't Show This Again", &dontShowModal);

			ImGui::EndPopup();
		}
		
		ImGui::PopID();
		ImGui::NextColumn();
	}

	ImGui::PopStyleColor();
	ImGui::Columns(1);

	if (!ImGui::IsAnyItemHovered() )
	{
		if (ImGui::IsWindowHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right))
			ImGui::OpenPopup("ContentBrowserMenu");
	}

	if (ImGui::BeginPopup("ContentBrowserMenu"))
	{
		if (ImGui::MenuItem("Import glTF"))
			m_enableImportOptions = true;

		if (ImGui::MenuItem("New Folder"))
			std::filesystem::create_directory(m_CurrentDirectory / "New Folder");

		ImGui::EndPopup();
	}
}

void NexusEd::ContentBrowser::DrawImportOptions()
{
	if (ImGui::Begin("GLTFImport"))
	{
		static AssetFilePath FilePath;
		static char FileName[256];

		ImGui::PushItemWidth(ImGui::GetContentRegionMax().x - 125.f);
		ImGui::InputText("Name", FileName, sizeof(FileName));
		ImGui::LabelText("Path", FilePath.generic_string().c_str());
		ImGui::PopItemWidth();

		ImGui::SameLine();

		if (ImGui::Button("Browse", ImVec2(85.f, 25.f)))
		{
			std::string path = FileDialog::OpenFile("glTF File(*.gltf)\0*.gltf\0");
			if (!path.empty())
			{
				FilePath = path;
			}
		}

		if (ImGui::Button("Import", ImVec2(85.f, 25.f)))
		{
			if (!FilePath.empty())
			{
				Importer::ImportGLTF(FilePath, m_CurrentDirectory, FileName);

				FilePath.clear();
				memset(FileName, 0, sizeof(FileName));

				m_enableImportOptions = false;
			}
		}

		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(85.f, 25.f)))
		{
			FilePath.clear();
			memset(FileName, 0, sizeof(FileName));

			m_enableImportOptions = false;
		}

		ImGui::End();
	}
}
