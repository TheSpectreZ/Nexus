#include "Panels.h"
#include "imgui.h"

#include "glad/glad.h"
#include "Utility/Logger.h"
#include "Utility/ResourceLoader.h"

#include "Platform/FileDialog.h"

void ImGuiImageTexture::Create(const char* file, uint32_t channels)
{
	Nexus::Utility::Image image;
	Nexus::Utility::ResourceLoader::LoadImage(&image, file, channels);

	GLenum internalFormat = 0, dataFormat = 0;
	if (image.channels == 4)
	{
		internalFormat = GL_RGBA8;
		dataFormat = GL_RGBA;
	}
	else if (image.channels == 3)
	{
		internalFormat = GL_RGB8;
		dataFormat = GL_RGB;
	}

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, image.width, image.height, 0, dataFormat, GL_UNSIGNED_BYTE, image.pixels);

	Nexus::Utility::ResourceLoader::FreeImage(&image);
}

void ImGuiImageTexture::Bind()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, id);
}

void PanelRenderer::Initialize()
{
	m_state = PanelState::MAIN_MENU;

	m_NexusTextureId.Create("res/textures/Nexus.png", 4);

}

void PanelRenderer::Render()
{
	ImGui::PushStyleColor(ImGuiCol_Button | ImGuiCol_Border, ImVec4(0.635f, 0.35f, 0.9f, 1.f));
	
	switch (m_state)
	{
	case PanelRenderer::PanelState::MAIN_MENU:
		MainMenu();
		break;
	case PanelRenderer::PanelState::CREATE_MENU:
		Create();
		break;
	case PanelRenderer::PanelState::OPEN_MENU:
		Open();
		break;
	default:
		break;
	}
	
	ImGui::PopStyleColor();
}

void PanelRenderer::Destroy()
{

}

void PanelRenderer::MainMenu()
{
	ImGui::Begin("MainMenu");
	
	ImVec2 size = ImGui::GetContentRegionMax();

	m_NexusTextureId.Bind();
	ImGui::Image((ImTextureID)m_NexusTextureId.Get(), {size.x, size.y * 2 / 5});

	ImGui::BeginChild("Child1");
	
	ImGui::SetCursorPos({ (size.x - 200) / 2.f, 20.f });
	if (ImGui::Button("New", { 200.f,50.f }))
	{
		m_EditorProject.Clear();
		m_state = PanelState::CREATE_MENU;
	}
	
	ImGui::SetCursorPos({ (size.x - 200) / 2.f, 90.f });
	if(ImGui::Button("Open", { 200.f,50.f }))
	{
		m_EditorProject.Clear();
		m_state = PanelState::OPEN_MENU;
	}

	ImGui::EndChild();
	
	ImGui::End();
}

void PanelRenderer::Create()
{
	ImGui::Begin("Create");
	ImVec2 size = ImGui::GetContentRegionMax();

	static char buffer[128];

	ImGui::SetCursorPos({ (size.x - 500.f) / 2.f,100.f });

	ImGui::BeginChild("Child2", { 500,400 });

	ImGui::PushItemWidth(350.f);

	if (ImGui::InputText("Project Name", buffer, sizeof(buffer)))
	{
		m_EditorProject.m_ProjectName = buffer;
	}
	ImGui::PopItemWidth();

	ImGui::SetCursorPosY(40.f);

	static std::filesystem::path Rootpath;

	ImGui::LabelText("Root Path", Rootpath.string().c_str());
	ImGui::SameLine();
	if (ImGui::Button("Browse"))
	{
		std::string path = Nexus::Platform::FileDialog::SelectFolder();
		if (!path.empty())
		{
			Rootpath = std::filesystem::path(path);
		}
	}

	ImGui::SetCursorPosY(300.f);

	ImGui::Indent(100.f);

	if (ImGui::Button("Back", { 100.f,50.f }))
	{
		m_state = PanelState::MAIN_MENU;
	}
	
	ImGui::SameLine(200.f, 90.f);

	if (ImGui::Button("Create", { 100.f,50.f }))
	{
		m_EditorProject.Serialize(Rootpath);
		m_state = PanelState::OPEN_MENU;
	}

	ImGui::EndChild();
	
	ImGui::End();
}

void PanelRenderer::Open()
{
	ImGui::Begin("Open");
	ImVec2 size = ImGui::GetContentRegionMax();

	ImGui::SetCursorPos({ (size.x - 700.f) / 2.f,100.f });

	if (ImGui::Button("Back"))
	{
		m_state = PanelState::MAIN_MENU;
	}

	ImGui::End();
}