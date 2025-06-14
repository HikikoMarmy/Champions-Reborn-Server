#pragma once

#include <memory>
#include <mutex>
#include <thread>
#include <atomic>
#include <vector>
#include <string>
#include <array>

//#define FD_SETSIZE 512
#include <winsock2.h>
#include <ws2tcpip.h>

#include "../Common/Constant.h"
#include "../Common/ByteStream.h"
#include "../Network/RealmSocket.h"

class LobbyServer
{
private:
	std::atomic< bool > m_running;
	std::thread m_thread;

public:
	static LobbyServer& Get()
	{
		static LobbyServer instance;
		return instance;
	}

	LobbyServer( const LobbyServer & ) = delete;
	LobbyServer &operator=( const LobbyServer & ) = delete;
	LobbyServer();
	~LobbyServer();

	void Start( std::string ip );
	void Stop();
	bool isRunning() const
	{
		return m_running;
	}

private:
	// These are the main sockets for the lobby server
	SOCKET m_conSocket;
	SOCKET m_rtaSocket;

	// Gateway sockets. 1 port per region.
	std::array< SOCKET, 8 > m_conGatewaySocket;
	std::array< SOCKET, 8 > m_rtaGatewaySocket;

	// Connected client sockets
	std::vector< sptr_socket > m_clientSockets;

	std::vector< uint8_t > m_recvBuffer;

	SOCKET OpenNetworkSocket( std::string ip, int32_t port );

	void Run();
	void CheckSocketProblem();
	void AcceptGateway( SOCKET socket, RealmGameType clientType );
	void AcceptClient( SOCKET socket, RealmGameType clientType );
	void ReadSocket( sptr_socket socket );
	void WriteSocket( sptr_socket socket );
	void HandleRequest( sptr_socket socket, sptr_byte_stream stream );
};