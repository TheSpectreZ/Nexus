#pragma once
#include <cstdint>
#include "Serializer/EditorProject.h"
#include "Serializer/LauncherConfig.h"

struct ImGuiImageTexture
{
	uint32_t id;
	
	void Create(const char* file, uint32_t channels);
	void Bind();
	uint32_t Get() { return id; }
};

class PanelRenderer
{
public:
	void Initialize();
	void Render();
	void Destroy();
private:
	void MainMenu();
	void Create();
	void Open();

	Nexus::Serializer::EditorProject m_EditorProject;

	ImGuiImageTexture m_NexusTextureId;

	enum class PanelState
	{
		NONE = 0,
		MAIN_MENU = 1,
		CREATE_MENU = 2,
		OPEN_MENU = 3
	};

	PanelState m_state;
};