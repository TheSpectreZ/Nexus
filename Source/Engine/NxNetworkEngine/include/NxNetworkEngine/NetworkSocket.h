#pragma once
#include "NxCore/Base.h"

#ifdef NEXUS_NETWORK_ENGINE_SHARED_BUILD
#define NEXUS_NETWORK_ENGINE_API __declspec(dllexport)
#else
#define NEXUS_NETWORK_ENGINE_API __declspec(dllimport)
#endif

namespace Nexus
{
	// Currently Server Socket connnects to only one Client Socket

	class NEXUS_NETWORK_ENGINE_API NetworkSocket
	{
	public:
		virtual ~NetworkSocket();
	protected:
		NetworkSocket(int type, int protocol);
	protected:
		struct INTERNAL;
		INTERNAL* p_State = nullptr;
	};

	class NEXUS_NETWORK_ENGINE_API ClientNetworkSocket_TCP : public NetworkSocket
	{
	public:
		ClientNetworkSocket_TCP();
		~ClientNetworkSocket_TCP() override = default;

		bool Connect();

		int Receive(char* buffer, int size);
		int Send(const char* buffer, int size);
	};
	
	class NEXUS_NETWORK_ENGINE_API ServerNetworkSocket_TCP : public NetworkSocket
	{
	public:
		ServerNetworkSocket_TCP();
		~ServerNetworkSocket_TCP() override = default;

		void Accept();

		int Receive(char* buffer, int size);
		int Send(const char* buffer, int size);
	};
	
	class NEXUS_NETWORK_ENGINE_API ClientNetworkSocket_UDP : public NetworkSocket
	{
	public:
		ClientNetworkSocket_UDP();
		~ClientNetworkSocket_UDP() override = default;

		int Receive(char* buffer, int size);
		int Send(const char* buffer, int size);
	};
	
	class NEXUS_NETWORK_ENGINE_API ServerNetworkSocket_UDP : public NetworkSocket
	{
	public:
		ServerNetworkSocket_UDP();
		~ServerNetworkSocket_UDP() override = default;

		int Receive(char* buffer, int size);
		int Send(const char* buffer, int size);
	};
}
