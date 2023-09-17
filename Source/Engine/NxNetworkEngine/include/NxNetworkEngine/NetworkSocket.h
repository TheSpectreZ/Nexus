#pragma once
#include "NxCore/Base.h"

#ifdef NEXUS_NETWORK_ENGINE_SHARED_BUILD
#define NEXUS_NETWORK_ENGINE_API __declspec(dllexport)
#else
#define NEXUS_NETWORK_ENGINE_API __declspec(dllimport)
#endif

namespace Nexus
{
	class NEXUS_NETWORK_ENGINE_API NetworkSocket
	{
	public:
		virtual ~NetworkSocket();
	protected:
		NetworkSocket();
	protected:
		struct INTERNAL;
		INTERNAL* p_State = nullptr;
	};
	
	class NEXUS_NETWORK_ENGINE_API ClientAddress
	{
		friend class ServerNetworkSocket;
	public:
		ClientAddress();
		~ClientAddress();

		ClientAddress(const ClientAddress& addr);

		void Clear();
	private:
		struct INTERNAL;
		INTERNAL* m_State = nullptr;
	};

	class NEXUS_NETWORK_ENGINE_API ClientNetworkSocket : public NetworkSocket
	{
	public:
		ClientNetworkSocket() = default;
		~ClientNetworkSocket() override = default;

		void Connect(const char* IPv4Address);

		int Receive(char* buffer, int size);
		int Send(const char* buffer, int size);
	};
	
	class NEXUS_NETWORK_ENGINE_API ServerNetworkSocket : public NetworkSocket
	{
	public:
		ServerNetworkSocket();
		~ServerNetworkSocket() override = default;

		int Receive(ClientAddress* address, char* buffer, int size);
		int Send(ClientAddress* address, const char* buffer, int size);
	};
}
