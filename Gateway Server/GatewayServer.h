#pragma once

#include <memory>

class GatewayServer
{
	static inline std::unique_ptr< GatewayServer > m_instance;

public:
	static GatewayServer& Get()
	{
		if( m_instance == nullptr )
		{
			m_instance.reset( new GatewayServer() );
		}

		return *m_instance;
	}

	GatewayServer();
	~GatewayServer();

	void Start( std::string ip, int32_t port );
	void Stop();
	bool isRunning() const
	{
		return m_running;
	}

private:
	std::atomic< bool > m_running;
	std::thread m_thread;

	SOCKET m_listenSocket;
	std::vector< sptr_socket > m_clientSockets;
	std::vector< uint8_t > m_recvBuffer;

	void Run();
	void AcceptNewClient();

	void ReadSocket( sptr_socket socket );
	void WriteSocket( sptr_socket socket );
	void HandleRequest( sptr_socket socket, sptr_byte_stream stream );
};