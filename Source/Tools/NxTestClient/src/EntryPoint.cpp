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

	NEXUS_LOG("Test Client", "Initialized");

	{
		std::mutex socketMutex;
		ClientNetworkSocket_TCP socket;

		NEXUS_LOG("Test Client", "Connecting to Server...");

		while (!socket.Connect());
		NEXUS_LOG("Test Client", "Connection Successfull");

		std::string msg;

		while (true)
		{
			std::cin >> msg;

			if (!msg.empty())
			{
				socket.Send(msg.c_str(), sizeof(msg));
				NEXUS_LOG("[ ME ]", "%s", msg.c_str());
			}
			
			if (msg == "-exit")
				break;
		}

		NEXUS_LOG("Test Client", "Closing Connection");
	}

	std::cout << "Press any key to Close";
	std::cin.get(); // Just To Pause the app before shutdown

	NetworkEngine::Shutdown();
	LogManager::Shutdown();
}