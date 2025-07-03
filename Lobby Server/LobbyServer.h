#pragma once

#include <memory>
#include <mutex>
#include <thread>
#include <atomic>
#include <vector>
#include <string>
#include <array>

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
	sptr_socket m_conSocket;
	sptr_socket m_rtaSocket;
	std::vector< sptr_socket > m_gatewaySockets;
	std::vector< sptr_socket > m_clientSockets;

	std::vector< uint8_t > m_recvBuffer;

	sptr_socket OpenListenerSocket( std::string ip, int32_t port, RealmGameType type );

	void Run();
	void CheckSocketProblem();
	void AcceptConnection( sptr_socket srcSocket );
	void ReadSocket( sptr_socket socket );
	void WriteSocket( sptr_socket socket );
	void HandleRequest( sptr_socket socket, sptr_byte_stream stream );
};