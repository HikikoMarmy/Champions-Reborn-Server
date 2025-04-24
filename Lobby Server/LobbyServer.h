#pragma once

#include <memory>
#include <mutex>

class LobbyServer
{
private:
	static inline std::unique_ptr< LobbyServer > m_instance;
	static inline std::mutex m_mutex;

	std::atomic< bool > m_running;
	std::thread m_thread;

public:
	static LobbyServer& Get()
	{
		std::lock_guard< std::mutex > lock( m_mutex );
		if( m_instance == nullptr )
		{
			m_instance.reset( new LobbyServer() );
		}

		return *m_instance;
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
	SOCKET m_conSocket;
	SOCKET m_rtaSocket;
	std::vector< sptr_socket > m_clientSockets;
	std::vector< uint8_t > m_recvBuffer;

	SOCKET OpenNetworkSocket( std::string ip, int32_t port );

	void Run();
	void CheckSocketProblem();
	void AcceptNewClient( SOCKET socket, RealmClientType clientType );
	void ReadSocket( sptr_socket socket );
	void WriteSocket( sptr_socket socket );
	void HandleRequest( sptr_socket socket, sptr_byte_stream stream );
};