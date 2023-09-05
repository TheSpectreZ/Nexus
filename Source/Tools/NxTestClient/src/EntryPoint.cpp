#include "NxNetworkEngine/NetworkEngine.h"
#include "NxNetworkEngine/NetworkSocket.h"

#include "NxCore/Logger.h"
#include <Windows.h>
#include <iostream>
#include <mutex>
#include <thread>

#define UDP 0
#define TCP 1

#define CONNECTION UDP

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	using namespace Nexus;

	LogManager::Initialize();
	LogManager::Get()->Make(LoggerType::Console);

	NetworkEngine::Initialize();

	NEXUS_LOG("Test Client", "Initialized");

	std::string name;
	std::cin >> name;

	{
#if CONNECTION 
		ClientNetworkSocket_TCP socket;
#else
		ClientNetworkSocket_UDP socket;
#endif // CONNECTION 
		
		NEXUS_LOG("Test Client", "Connecting to Server...");

#if CONNECTION
		while (!socket.Connect());
		NEXUS_LOG("Test Client", "Connection Successfull");
#endif // CONNECTION

		std::string msg;

		while (true)
		{
			std::cin >> msg;

			if (!msg.empty())
			{
				if (msg == "-exit")
					break;

				std::string s = name + ": " + msg;
				socket.Send(s.c_str(), sizeof(s));

				NEXUS_LOG("[ ME ]", "%s", msg.c_str());
			}
		}

		NEXUS_LOG("Test Client", "Closing Connection");
	}

	NetworkEngine::Shutdown();
	LogManager::Shutdown();
}