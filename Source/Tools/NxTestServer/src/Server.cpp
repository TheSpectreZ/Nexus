#include "NxApplication/EntryPoint.h"
#include "NxApplication/Layer.h"

#include "NxCore/Logger.h"	
#include "NxImGui/Context.h"
#include "NxNetworkEngine/NetworkSocket.h"

class ServerLayer : public Nexus::Layer
{
	Nexus::Ref<Nexus::ServerNetworkSocket> m_Socket;
	
	std::vector<std::string> m_Logs;
	std::vector<Nexus::ClientAddress> m_Clients;
public:
	ServerLayer() = default;
	~ServerLayer() override = default;

	void OnAttach() override
	{
		auto app = Nexus::Application::Get();
		NxImGui::Context::Initialize(app->GetWindow(), app->GetAppSpecs().rApi);

		m_Socket = Nexus::CreateRef<Nexus::ServerNetworkSocket>();
	}

	void OnUpdate(float dt) override
	{
		static char buffer[1024]{};
		static Nexus::ClientAddress addr;

		if (m_Socket->Receive(&addr, buffer, sizeof(buffer)))
		{
			std::string JsonStr = buffer;
			int JsonSize = sizeof(char) * JsonStr.size();

			nlohmann::json Json = nlohmann::json::parse(JsonStr);

			if (Json["Request"].get<std::string>() == "Join")
			{
				m_Clients.push_back(addr);
				addr.Clear();
			}
			else if (Json["Request"].get<std::string>() == "Message")
			{
				auto msg = Json["Data"].get<std::string>();

				for (auto& c : m_Clients)
					m_Socket->Send(&addr, JsonStr.c_str(), JsonSize);

				m_Logs.push_back(msg);
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
					std::string Istr = std::format("SERVER: {}", input);

					nlohmann::json Json;
					Json["Request"] = "Message";
					Json["Data"] = Istr;

					std::string JsonStr = Json.dump();
					int JsonSize = sizeof(char) * JsonStr.size();

					for (auto& c : m_Clients)
					{
						// Need to check every return value
						m_Socket->Send(&c, JsonStr.c_str(), JsonSize);
					}

					m_Logs.push_back(Istr);
					
					memset(input, 0, sizeof(input));
					inputBool = false;
				}
			}

			ImGui::End();
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

class ServerApp : public Nexus::Application
{
public:
	ServerApp()
	{
		m_AppSpecs.Window_Title = "TestServer";
		m_AppSpecs.Window_Width = 800;
		m_AppSpecs.Window_height = 600;
		m_AppSpecs.rApi = Nexus::RendererAPI::VULKAN;

		PushLayer(new ServerLayer);
	}
};

Nexus::Application* CreateApplication(const std::unordered_map<std::string, std::string>& CCmap)
{
	return new ServerApp;
}