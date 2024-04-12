// STL
#include <vector>
#include <chrono>

// GLFW
#include "GLFW/glfw3.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3native.h"

// Core
#include "NxCore/Base.h"
#include "NxCore/Logger.h"
#include "NxCore/Assertion.h"

// Application
#include "NxGameEngine/Engine.h"

// Modules
#include "NxRenderEngine/Renderer.h"

namespace Nexus::Utils
{
	static const std::string WHITESPACE = " \n\r\t\f\v";

	std::string ltrim(const std::string& s)
	{
		size_t start = s.find_first_not_of(WHITESPACE);
		return (start == std::string::npos) ? "" : s.substr(start);
	}

	std::string rtrim(const std::string& s)
	{
		size_t end = s.find_last_not_of(WHITESPACE);
		return (end == std::string::npos) ? "" : s.substr(0, end + 1);
	}

	std::string trim(const std::string& s)
	{
		return rtrim(ltrim(s));
	}

	std::unordered_map<std::string, std::string> BuildCCMap(const std::string& args)
	{
		std::vector<std::string> items;
		{
			std::stringstream ss(args);
			std::string line;

			while (std::getline(ss, line, ' '))
				items.push_back(line);
		}

		std::unordered_map<std::string, std::string> map;
		for (auto& arg : items)
		{
			auto trimmed = trim(arg);

			if (trimmed.length() > 1 && trimmed[0] == '-')
			{
				auto curEquals = trimmed.find_first_of('=');

				if (curEquals == std::string::npos)
				{
					auto curKey = trimmed.substr(1);
					map[curKey] = "true";
				}
				else if (curEquals != std::string::npos && curEquals > 1)
				{
					auto curKey = trimmed.substr(1, curEquals - 1);
					auto curValue = trimmed.substr(curEquals + 1);
					map[curKey] = curValue;
				}
			}
		}

		return map;
	}

}

Nexus::GameEngine::GameEngine()
{
	LogManager::Init();
	LogManager::Get()->Make(LoggerType::Console);
	LogManager::Get()->Make(LoggerType::File);

	Windowing::Init();
	
	mWindow.eventHandler.OnResize = NEXUS_BIND_FN(OnScreenResize, this);
	mWindow.eventHandler.OnKeyPress = NEXUS_BIND_FN(OnKeyPress, this);
	mWindow.eventHandler.OnMousePress = NEXUS_BIND_FN(OnMousePress, this);
}

Nexus::GameEngine::~GameEngine()
{
	Windowing::Shut();
	LogManager::Shut();
}

bool Nexus::GameEngine::Init(const GameEngineSpecifications& specs)
{
	auto ccMap = Utils::BuildCCMap(specs.cmdlineArgs);

	mWindow.width = specs.window_width;
	mWindow.height = specs.window_height;
	mWindow.title = specs.window_title;

	bool bSuccess = true;
	
	bSuccess &= Windowing::Create(&mWindow);
	bSuccess &= mRenderEngine.Intialize(RenderEngineSpecifications
		{
			.eAPI = specs.eAPI,
			.pWindow = &mWindow,
			.hInstance = specs.hInstance
		});

	if (!bSuccess) Shut();
	return bSuccess;
}

void Nexus::GameEngine::Run()
{
	for (auto& layer : mLayerStack)
	{
		layer->Create();
	}

	Windowing::Show(&mWindow);
	while (Windowing::IsOpen(&mWindow))
	{
		Windowing::PollEvents();

		auto dt = (float)Windowing::GetDeltaTime();
		for (auto& layer : mLayerStack)
		{
			layer->Update(dt);
		}

		mRenderEngine.Flush();
	}
	mRenderEngine.WaitForGPU();
}

void Nexus::GameEngine::Shut()
{
	for (auto& layer : mLayerStack)
	{
		layer->Destroy();
		delete layer;
	}

	mRenderEngine.Terminate();
	Windowing::Destroy(&mWindow);
}

void Nexus::GameEngine::OnScreenResize(int width, int height)
{
	mRenderEngine.OnScreenResize(width, height);

	for (auto& layer : mLayerStack)
	{
		if (layer->OnWindowResize(width, height))
			break;
	}
}

void Nexus::GameEngine::OnKeyPress(Key::Code key, InputAction action, InputModifier mods)
{
	for (auto& layer : mLayerStack)
	{
		if (layer->OnKeyPress(key, action, mods))
			break;
	}
}

void Nexus::GameEngine::OnMousePress(Mouse::Code button, InputAction action, InputModifier mods)
{
	for (auto& layer : mLayerStack)
	{
		if (layer->OnMousePress(button, action, mods))
			break;
	}
}
