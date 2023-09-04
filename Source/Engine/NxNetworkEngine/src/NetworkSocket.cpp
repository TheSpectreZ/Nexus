#include "NxNetworkEngine/NetworkSocket.h"
#include "NxCore/Assertion.h"

#include <WinSock2.h>
#include <WS2tcpip.h>

#define NEXUS_NETWORK_PORT 54321
#define NEXUS_HOST_NAME ""

struct Nexus::NetworkSocket::INTERNAL
{
	// - socket handle
	SOCKET socket;

	// - socket address
	sockaddr_in sockAddr;
};

Nexus::NetworkSocket::NetworkSocket(int type, int protocol)
{
	p_State = new INTERNAL;
	p_State->socket = socket(AF_INET, type, protocol);

	p_State->sockAddr.sin_family = AF_INET;
	p_State->sockAddr.sin_port = htons(NEXUS_NETWORK_PORT);
}

Nexus::NetworkSocket::~NetworkSocket()
{
	closesocket(p_State->socket);
	delete p_State;
}

Nexus::ClientNetworkSocket_TCP::ClientNetworkSocket_TCP()
	:NetworkSocket(SOCK_STREAM,IPPROTO_TCP)
{	
	auto host = gethostbyname(NEXUS_HOST_NAME);
	auto IP = inet_ntoa(*(in_addr*)*host->h_addr_list);

	p_State->sockAddr.sin_addr.s_addr = inet_addr(IP);
}

bool Nexus::ClientNetworkSocket_TCP::Connect()
{
	int result = connect(p_State->socket, (sockaddr*)&p_State->sockAddr, sizeof(p_State->sockAddr));
	return !(result == SOCKET_ERROR);
}

int Nexus::ClientNetworkSocket_TCP::Receive(char* buffer, int size)
{
	return recv(p_State->socket, buffer, size, 0);
}

int Nexus::ClientNetworkSocket_TCP::Send(const char* buffer, int size)
{
	return send(p_State->socket, buffer, size, 0);
}

Nexus::ServerNetworkSocket_TCP::ServerNetworkSocket_TCP()
	:NetworkSocket(SOCK_STREAM, IPPROTO_TCP)
{
	p_State->sockAddr.sin_addr.s_addr = INADDR_ANY;

	bind(p_State->socket, (sockaddr*)&p_State->sockAddr, sizeof(p_State->sockAddr));
	listen(p_State->socket, SOMAXCONN);
}

void Nexus::ServerNetworkSocket_TCP::Accept()
{
	SOCKET sock{};
	sockaddr_in addr{};
	int addrSize = sizeof(addr);

	sock = accept(p_State->socket, (sockaddr*)&addr, &addrSize);
	NEXUS_ASSERT(sock == INVALID_SOCKET, "Socket Accept Failed");

	closesocket(p_State->socket);
	p_State->socket = sock;
}

int Nexus::ServerNetworkSocket_TCP::Receive(char* buffer, int size)
{
	return recv(p_State->socket, buffer, size, 0);
}

int Nexus::ServerNetworkSocket_TCP::Send(const char* buffer, int size)
{
	return send(p_State->socket, buffer, size, 0);
}

Nexus::ClientNetworkSocket_UDP::ClientNetworkSocket_UDP()
	:NetworkSocket(SOCK_DGRAM, IPPROTO_UDP)
{
	auto host = gethostbyname(NEXUS_HOST_NAME);
	auto IP = inet_ntoa(*(in_addr*)*host->h_addr_list);
	
	p_State->sockAddr.sin_addr.s_addr = inet_addr(IP);
}

int Nexus::ClientNetworkSocket_UDP::Receive(char* buffer, int size)
{
	sockaddr_in addr{};
	int addrSize = sizeof(addr);

	return recvfrom(p_State->socket, buffer, size, 0, (sockaddr*)&addr, &addrSize);
}

int Nexus::ClientNetworkSocket_UDP::Send(const char* buffer, int size)
{
	return sendto(p_State->socket, buffer, size, 0, (sockaddr*)&p_State->sockAddr, sizeof(p_State->sockAddr));
}

Nexus::ServerNetworkSocket_UDP::ServerNetworkSocket_UDP()
	:NetworkSocket(SOCK_DGRAM, IPPROTO_UDP)
{
	p_State->sockAddr.sin_addr.s_addr = INADDR_ANY;
	bind(p_State->socket, (sockaddr*)&p_State->sockAddr, sizeof(p_State->sockAddr));
}

int Nexus::ServerNetworkSocket_UDP::Receive(char* buffer, int size)
{
	sockaddr addr{};
	int addrSize = sizeof(addr);
	
	return recvfrom(p_State->socket, buffer, size, 0, (sockaddr*)&addr, &addrSize);
}

int Nexus::ServerNetworkSocket_UDP::Send(const char* buffer, int size)
{
	sockaddr_in addr{};
	return sendto(p_State->socket, buffer, size, 0, (sockaddr*)&addr, sizeof(addr));
}
