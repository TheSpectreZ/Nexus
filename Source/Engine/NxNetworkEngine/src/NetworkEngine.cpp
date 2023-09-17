#include "NxNetworkEngine/NetworkEngine.h"
#include "NxCore/Assertion.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

Nexus::NetworkEngine* Nexus::NetworkEngine::s_Instance = nullptr;

void Nexus::NetworkEngine::Initialize()
{
	s_Instance = new NetworkEngine;

	WSADATA wsaData{};

	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	NEXUS_ASSERT(result != 0, "WSA Startup Failed: Error-%i", result);

	NEXUS_LOG("NETWORK", "Network Engine Initialized");
}

void Nexus::NetworkEngine::Shutdown()
{
	WSACleanup();
	delete s_Instance;

	NEXUS_LOG("NETWORK", "Network Engine Shutdown");
}
