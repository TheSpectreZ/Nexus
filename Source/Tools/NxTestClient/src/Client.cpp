#include "NxApplication/EntryPoint.h"
#include "NxApplication/Layer.h"

#include "NxCore/Logger.h"	
#include "NxImGui/Context.h"
#include "NxNetworkEngine/NetworkSocket.h"

#include "nlohmann/json.hpp"

class ClientLayer : public Nexus::Layer
{
	Nexus::Ref<Nexus::ClientNetworkSocket> m_Socket;
	std::vector<std::string> m_Logs;

	bool m_Connected = false;
	std::string m_Name;
public:
	ClientLayer() = default;
	~ClientLayer() override = default;

	void OnAttach() override
	{
		auto app = Nexus::Application::Get();
		NxImGui::Context::Initialize(app->GetWindow(), app->GetAppSpecs().rApi);

		m_Socket = Nexus::CreateRef<Nexus::ClientNetworkSocket>();
	}

	void OnUpdate(float dt) override
	{
		if (m_Connected)
		{
			static char buffer[1024]{};

			if (m_Socket->Receive(buffer, sizeof(buffer)))
			{
				std::string JsonStr = buffer;
				nlohmann::json Json = nlohmann::json::parse(JsonStr);

				if (Json["Request"].get<std::string>() == "Message")
				{
					auto msg = Json["Data"].get<std::string>();
					m_Logs.push_back(msg);
				}
			}
		}
	}

	void OnRender() override
	{
		NxImGui::Context::StartFrame();
		
		if (ImGui::Begin("Console"))
		{
			for (auto& log : m_Logs)
			{
				ImGui::TextUnformatted(log.c_str());
			}

			if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
				ImGui::SetScrollHereY(1.0f);

			ImGui::End();
		}
				
		if (ImGui::Begin("Input"))
		{
			static char input[256]{};
			static bool inputBool = false;

			if (ImGui::InputText("##FileNameInput", input, sizeof(input)))
				inputBool = true;

			if (ImGui::Button("Send"))
			{
				if (inputBool)
				{
					std::string Istr = std::format("{}: {}", m_Name.c_str(), input);

					nlohmann::json Json;
					Json["Request"] = "Message";
					Json["Data"] = Istr;

					std::string JsonStr = Json.dump();
					int JsonSize = sizeof(char) * JsonStr.size();

					if ( m_Connected && m_Socket->Send(JsonStr.c_str(), JsonSize) > 0)
					{
						memset(input, 0, sizeof(input));
						inputBool = false;
					}
				}
			}

			ImGui::End();
		}
	
		if (!m_Connected)
		{
			ImGui::OpenPopup("Setup");

			ImVec2 center = ImGui::GetMainViewport()->GetCenter();
			ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		}

		if (ImGui::BeginPopupModal("Setup", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			static char nameBuffer[256];
			static char IpBuffer[256];

			ImGui::InputText("Name", nameBuffer, sizeof(nameBuffer));
			ImGui::InputText("IpAddress", IpBuffer, sizeof(nameBuffer));

			if (ImGui::Button("Connect"))
			{
				m_Socket->Connect(IpBuffer);

				nlohmann::json Json;
				Json["Request"] = "Join";
				
				m_Name = nameBuffer;

				std::string JsonStr = Json.dump();
				int JsonSize = sizeof(char) * JsonStr.size();

				m_Socket->Send(JsonStr.c_str(), JsonSize);

				m_Connected = true;
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		NxImGui::Context::EndFrame();
	}

	void OnDetach() override
	{
		NxImGui::Context::Shutdown();
	}

	void OnWindowResize(int width, int height) override
	{
		NxImGui::Context::OnWindowResize({ (uint32_t)width,(uint32_t)height });
	}
};

class ClientApp : public Nexus::Application
{
public:
	ClientApp()
	{
		m_AppSpecs.Window_Title = "TestClient";
		m_AppSpecs.Window_Width = 800;
		m_AppSpecs.Window_height = 600;
		m_AppSpecs.rApi = Nexus::RendererAPI::VULKAN;

		PushLayer(new ClientLayer);
	}
};

Nexus::Application* CreateApplication(const std::unordered_map<std::string, std::string>& CCmap)
{
	return new ClientApp;
}