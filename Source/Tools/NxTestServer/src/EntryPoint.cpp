#include "NxNetworkEngine/NetworkEngine.h"
#include "NxNetworkEngine/NetworkSocket.h"

#include "NxCore/Logger.h"
#include <Windows.h>
#include <iostream>
#include <mutex>
#include <thread>

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	using namespace Nexus;

	LogManager::Initialize();
	LogManager::Get()->Make(LoggerType::Console);

	NetworkEngine::Initialize();

	NEXUS_LOG("Test Server", "Initialized");

	{
		std::mutex socketMutex;
		ServerNetworkSocket_TCP socket;
		NEXUS_LOG("Test Server", "Waiting for Client...");

		socket.Accept();
		NEXUS_LOG("Test Server", "Connection Successfull");

		char buffer[1024]{};

		while (true)
		{
			if (socket.Receive(buffer, sizeof(char) * 1024) > 0)
			{
				if (!strcmp(buffer, "-exit"))
					break;

				NEXUS_LOG("[ CLIENT ]", "%s", buffer);
			}
		}

		NEXUS_LOG("Test Server", "Closing Connection");
	}

	std::cout << "Press any key to Close";
	std::cin.get(); // Just To Pause the app before shutdown

	NetworkEngine::Shutdown();
	LogManager::Shutdown();
}