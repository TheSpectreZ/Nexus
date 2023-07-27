#include "ContentBrowser.h"
#include "NxCore/Logger.h"

#include "NxAsset/Asset.h"
#include "NxAsset/Manager.h"

#include "NxGraphics/Meshing.h"
#include "NxRenderEngine/EnvironmentBuilder.h"

#include "NxApplication/FileDialog.h"
#include "NxImGui/Context.h"

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
		auto[res,id] = Importer::Loadimage("Resources/Icons/File.NxTex", fileImage);

		TextureSpecification specs{};
		specs.extent = { fileImage.width,fileImage.height };
		specs.pixels = fileImage.pixels.data();
		specs.format = TextureFormat::RGBA8_SRGB;
		specs.type = TextureType::TwoDim;
		specs.usage = TextureUsage::ShaderSampled;

		Ref<Texture> Texture = Module::AssetManager::Get()->Allocate<Nexus::Texture>(id, specs);
		m_FileID = NxImGui::Context::CreateTextureID(Texture, m_Sampler);
	}
	
	{		
		Meshing::Image folderImage;
		auto [res, id] = Importer::Loadimage("Resources/Icons/Folder.NxTex", folderImage);
		
		TextureSpecification specs{};
		specs.extent = { folderImage.width,folderImage.height };
		specs.pixels = folderImage.pixels.data();
		specs.format = TextureFormat::RGBA8_SRGB;
		specs.type = TextureType::TwoDim;
		specs.usage = TextureUsage::ShaderSampled;

		Ref<Texture> Texture = Module::AssetManager::Get()->Allocate<Nexus::Texture>(id, specs);
		m_FolderID = NxImGui::Context::CreateTextureID(Texture, m_Sampler);
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
	m_AssetDirectory = projectRootPath + "/Assets";
	
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
	{
		ImGui::Begin("Import");
		
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
				FilePath = path;
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

	NxImGui::Context::BindTextureID(m_FolderID);
	NxImGui::Context::BindTextureID(m_FileID);
	
	static AssetFilePath dragDropPayload, dragDropTarget;

	static bool showDeleteModal = false;
	static bool dontShowModal = false;

	static int Renaming = -1;
	static int PasteType = -1;

	int i = 0;
	for (auto& dir : std::filesystem::directory_iterator(path))
	{
		ImGui::PushID(i++);

		const auto& p = dir.path();
		auto filenameString = p.filename().string();

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

			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
			{
				const wchar_t* itemPath = p.c_str();
				ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
				ImGui::EndDragDropSource();
			}

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
				{
					dragDropPayload = (const wchar_t*)payload->Data;
					dragDropTarget = p;

					if (!dragDropPayload.empty() && !dragDropTarget.empty())
						ImGui::OpenPopup("DragDropPopup");
				}
				ImGui::EndDragDropTarget();
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

		// Doesnt work with folders yet
		if (ImGui::BeginPopup("DragDropPopup"))
		{
			if (ImGui::MenuItem("Copy"))
			{
				if (std::filesystem::is_directory(dragDropPayload))
					dragDropTarget /= dragDropPayload.filename();

				std::filesystem::copy(dragDropPayload, dragDropTarget, std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing);

				dragDropPayload.clear();
				dragDropTarget.clear();
			}

			if (ImGui::MenuItem("Move"))
			{
				if (std::filesystem::is_directory(dragDropPayload))
					dragDropTarget /= dragDropPayload.filename();

				std::filesystem::copy(dragDropPayload, dragDropTarget, std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing);
				std::filesystem::remove_all(dragDropPayload);

				dragDropPayload.clear();
				dragDropTarget.clear();
			}

			ImGui::EndPopup();
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

		if (ImGui::BeginPopup("FileOptions"))
		{
			if (ImGui::MenuItem("Rename"))
				Renaming = i;

			if (ImGui::MenuItem("Delete"))
				showDeleteModal = true;

			if (ImGui::MenuItem("Copy"))
			{
				dragDropPayload = p;
				PasteType = 0;
			}

			if (ImGui::MenuItem("Move"))
			{
				dragDropPayload = p;
				PasteType = 1;
			}

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

		if (PasteType > -1 && ImGui::MenuItem("Paste"))
		{
			dragDropTarget = m_CurrentDirectory;

			if (std::filesystem::is_directory(dragDropPayload))
				dragDropTarget /= dragDropPayload.filename();

			std::filesystem::copy(dragDropPayload, dragDropTarget, std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing);

			if(PasteType == 1)
				std::filesystem::remove_all(dragDropPayload);
			
			dragDropPayload.clear();
			dragDropTarget.clear();
			PasteType = -1;
		}

		ImGui::EndPopup();
	}
}
