#include "NxNetworkEngine/NetworkEngine.h"
#include "NxNetworkEngine/NetworkSocket.h"

#include "NxCore/Logger.h"
#include <Windows.h>
#include <iostream>

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	using namespace Nexus;

	LogManager::Initialize();
	LogManager::Get()->Make(LoggerType::Console);

	NetworkEngine::Initialize();

	NEXUS_LOG("Test Server", "Initialized");

	{
		ServerNetworkSocket_TCP socket;
		NEXUS_LOG("Test Server", "Waiting for Client...");

		socket.Accept();
		NEXUS_LOG("Test Server", "Connection Successfull");

		char buffer[1024]{};
		
		if (int res = socket.Receive(buffer, sizeof(char) * 1024); res > 0)
		{
			NEXUS_LOG("Test Server", "Message From Client: %s", buffer);

			std::string msg = "Heyy Client\n";

			socket.Send(msg.c_str(), msg.size());
			NEXUS_LOG("Test Server", "Message To Client: %s", msg.c_str());
		}

		NEXUS_LOG("Test Server", "Closing Connection");
	}

	std::cout << "Press any key to Close";
	std::cin.get(); // Just To Pause the app before shutdown

	NetworkEngine::Shutdown();
	LogManager::Shutdown();
}