#include <Windows.h>

#include "NxCore/StringUtils.h"
#include "NxGameEngine/Engine.h"
#include "EditorLayer.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	Nexus::GameEngineSpecifications specs{};
	specs.cmdlineArgs = Nexus::wstring_to_string(lpCmdLine);
	specs.hInstance = hInstance;
	specs.window_width = 1280;
	specs.window_height = 720;
	specs.window_title = "Nexus-Editor";
	specs.eAPI = Nexus::RenderEngineAPI::Vulkan;

	Nexus::GameEngine engine;
	engine.AttachLayer<Nexus::EditorLayer>();

	bool bSuccess = false;
	if (bSuccess = engine.Init(specs))
	{
		engine.Run();
		engine.Shut();
	}

	return (int)!bSuccess;
}