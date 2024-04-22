#include "EditorLayer.h"
#include "NxGameEngine/Engine.h"

#include "imgui.h"
#include "NxCore/Logger.h"
#include "NxCore/Event.h"

#include "NxRenderEngine/RenderSettings.h"

Nexus::EditorLayer::EditorLayer()
{
	mUIeventDispatcher.AddListener<ResolutionChangeEvent>(
		[this](ResolutionChangeEvent event)
		{
			mSceneDrawer->Resize(event.width, event.height);
			mUserInterface->AttachViewportPresentable(mSceneDrawer->getPresentable());
		});
}

void Nexus::EditorLayer::Create()
{
	auto re = mEngine->getRenderEngine();
	auto gd = re->getGraphicsDevice();

	mSceneDrawer = std::make_shared<SceneDrawer>(gd, nullptr, Resolution::e1440);
	
	// ImGui
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		ImGui::StyleColorsDark();

		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.f;
		}

		mUserInterface = gd->CreateUserInterface();
		mUserInterface->AttachViewportPresentable(mSceneDrawer->getPresentable());

		mContentBrowser.Initialize("Assets");
	}
}

void Nexus::EditorLayer::Update(float dt)
{
	mUIeventDispatcher.Dispatch();
}

void Nexus::EditorLayer::Render()
{
	auto gd = mEngine->getRenderEngine()->getGraphicsDevice();
	
	{
		gd->BeginFrame();

		// Engine
		{
			mSceneDrawer->Draw();
		}

		// ImGui Editor
		{
			mUserInterface->Begin();
			ImGui::DockSpaceOverViewport();
			
			RenderImGUI();
			
			mUserInterface->RenderViewport();
			mUserInterface->End();
		}

		gd->EndFrame(mUserInterface->getPresentable());
	}
}

void Nexus::EditorLayer::Destroy()
{
	{
		mContentBrowser.Destroy();
		mUserInterface.reset();

		ImGui::DestroyContext();
	}

	mSceneDrawer.reset();
}

void Nexus::EditorLayer::OnWindowResize(int width, int height)
{
	mUserInterface->OnWindowResize(width, height);
}

void Nexus::EditorLayer::OnKeyPress(Key::Code key, InputAction action, InputModifier mods)
{
}

void Nexus::EditorLayer::OnMousePress(Mouse::Code mouse, InputAction action, InputModifier mods)
{
}

void Nexus::EditorLayer::RenderImGUI()
{
	UI_RenderSettings();

	mContentBrowser.Render();
}

void Nexus::EditorLayer::UI_RenderSettings()
{
	ImGui::Begin("RenderSettings");
	{
		// Resolution Selection
		{
			static uint32_t item_index = IM_ARRAYSIZE(sResolutionString) - 1;
			if (ImGui::BeginCombo("Resolution", sResolutionString[item_index]))
			{
				for (int i = 0; i < IM_ARRAYSIZE(sResolutionString); i++)
				{
					if (ImGui::Selectable(sResolutionString[i]))
					{
						mUIeventDispatcher.Trigger<ResolutionChangeEvent>((Resolution)i);
						item_index = i;
					}
				}
				ImGui::EndCombo();
			}
		}
	}
	ImGui::End();
}
