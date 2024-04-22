// STL
#include  <sstream>

// Core
#include "NxCore/Base.h"
#include "NxCore/Logger.h"
#include "NxCore/Assertion.h"

// Application
#include "NxGameEngine/Engine.h"

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
	LogManager::Instance().Make<ConsoleLogger>();
	LogManager::Instance().Make<FileLogger>();

	Windowing::Init();
	
	mApplicationEventDispatcher.AddListener<WindowResizeEvent>(
		[this](WindowResizeEvent event)
		{
			this->OnScreenResize(event.width, event.height);
		}
	);

	mApplicationEventDispatcher.AddListener<KeyPressEvent>(
		[this](KeyPressEvent event)
		{
			this->OnKeyPress(event.key, event.action, event.mods);
		}
	);

	mApplicationEventDispatcher.AddListener<MousePressEvent>(
		[this](MousePressEvent event)
		{
			this->OnMousePress(event.mouse, event.action, event.mods);
		}
	);
}

Nexus::GameEngine::~GameEngine()
{
	Windowing::Shut();
}

bool Nexus::GameEngine::Init(const GameEngineSpecifications& specs)
{
	auto ccMap = Utils::BuildCCMap(specs.cmdlineArgs);

	mWindow.width = specs.window_width;
	mWindow.height = specs.window_height;
	mWindow.title = specs.window_title;
	mWindow.eventDispatcher = &mApplicationEventDispatcher;

	bool bSuccess = true;
	
	bSuccess &= Windowing::Create(&mWindow);
	bSuccess &= mRenderEngine.Initialize(RenderEngineSpecifications
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
	mApplicationLayer->Create();

	Windowing::Show(&mWindow);
	while (Windowing::IsOpen(&mWindow))
	{
		Windowing::PollEvents();
		mApplicationEventDispatcher.Dispatch();

		auto dt = (float)Windowing::GetDeltaTime();
		mApplicationLayer->Update(dt);

		mApplicationLayer->Render();	
		mRenderEngine.Flush();
	}
	mRenderEngine.WaitForGPU();
	
	mApplicationLayer->Destroy();
}

void Nexus::GameEngine::Shut()
{
	mApplicationLayer.reset();

	mRenderEngine.Terminate();
	Windowing::Destroy(&mWindow);
}

void Nexus::GameEngine::OnScreenResize(int width, int height)
{
	mRenderEngine.OnScreenResize(width, height);
	mApplicationLayer->OnWindowResize(width, height);
}

void Nexus::GameEngine::OnKeyPress(Key::Code key, InputAction action, InputModifier mods)
{
	mApplicationLayer->OnKeyPress(key, action, mods);
}

void Nexus::GameEngine::OnMousePress(Mouse::Code button, InputAction action, InputModifier mods)
{
	mApplicationLayer->OnMousePress(button, action, mods);
}
