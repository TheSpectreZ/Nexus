#include "NxNetworkEngine/NetworkSocket.h"
#include "NxCore/Assertion.h"

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <WinSock2.h>
#include <WS2tcpip.h>

#define NEXUS_NETWORK_PORT 54321

struct Nexus::NetworkSocket::INTERNAL
{
	SOCKET socket;
	sockaddr_in sockAddr;
};

Nexus::NetworkSocket::NetworkSocket()
{
	p_State = new INTERNAL;
	p_State->socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	unsigned long ul = 1;
	ioctlsocket(p_State->socket, FIONBIO, (unsigned long*)&ul);
	
	p_State->sockAddr.sin_family = AF_INET;
	p_State->sockAddr.sin_port = htons(NEXUS_NETWORK_PORT);
}

Nexus::NetworkSocket::~NetworkSocket()
{
	closesocket(p_State->socket);
	delete p_State;
}

struct Nexus::ClientAddress::INTERNAL
{
	SOCKET socket;
	sockaddr_in addr;
};

Nexus::ClientAddress::ClientAddress()
{
	m_State = new INTERNAL;
	m_State->socket = NULL;
}

Nexus::ClientAddress::ClientAddress(const ClientAddress& addr)
{
	if (!m_State)
		m_State = new INTERNAL;

	m_State->addr = addr.m_State->addr;
	m_State->socket = addr.m_State->socket;
}

Nexus::ClientAddress::~ClientAddress()
{
	Clear();
	delete m_State;
}

void Nexus::ClientAddress::Clear()
{
	m_State->socket = NULL;
	m_State->addr = sockaddr_in();
}

void Nexus::ClientNetworkSocket::Connect(const char* HostName)
{
	auto host = gethostbyname(HostName);
	auto IP = inet_ntoa(*(in_addr*)*host->h_addr_list);

	p_State->sockAddr.sin_addr.s_addr = inet_addr(IP);
}

int Nexus::ClientNetworkSocket::Receive(char* buffer, int size)
{
	sockaddr_in addr{};
	int addrSize = sizeof(addr);

	return recvfrom(p_State->socket, buffer, size, 0, (sockaddr*)&addr, &addrSize) != SOCKET_ERROR;
}

int Nexus::ClientNetworkSocket::Send(const char* buffer, int size)
{
	return sendto(p_State->socket, buffer, size, 0, (sockaddr*)&p_State->sockAddr, sizeof(p_State->sockAddr));
}

Nexus::ServerNetworkSocket::ServerNetworkSocket()
{
	p_State->sockAddr.sin_addr.s_addr = INADDR_ANY;
	bind(p_State->socket, (sockaddr*)&p_State->sockAddr, sizeof(p_State->sockAddr));
}

int Nexus::ServerNetworkSocket::Receive(ClientAddress* address, char* buffer, int size)
{
	int addrSize = sizeof(sockaddr);
	return recvfrom(p_State->socket, buffer, size, 0, (sockaddr*)&address->m_State->addr, &addrSize) != SOCKET_ERROR;
}

int Nexus::ServerNetworkSocket::Send(ClientAddress* address, const char* buffer, int size)
{
	return sendto(p_State->socket, buffer, size, 0, (sockaddr*)&address->m_State->addr, sizeof(sockaddr));
}