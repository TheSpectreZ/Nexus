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

	NEXUS_LOG("Test Client", "Initialized");

	{
		ClientNetworkSocket_TCP socket;
		NEXUS_LOG("Test Client", "Connecting to Server...");

		while (!socket.Connect());
		NEXUS_LOG("Test Client", "Connection Successfull");

		std::string testMsg = "Heyy Server!\n";
		socket.Send(testMsg.c_str(), sizeof(testMsg));
		NEXUS_LOG("Test Server", "Message To Server: %s", testMsg.c_str());
		
		char rbuf[1024]{};

		if (socket.Receive(rbuf, sizeof(char) * 1024) > 0)
		{
			NEXUS_LOG("Test Client", "Msg From Server: %s", rbuf);
		}

		NEXUS_LOG("Test Client", "Closing Connection");
	}

	std::cout << "Press any key to Close";
	std::cin.get(); // Just To Pause the app before shutdown

	NetworkEngine::Shutdown();
	LogManager::Shutdown();
}