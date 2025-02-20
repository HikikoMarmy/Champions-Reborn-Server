#pragma once

#include <memory>
#include <mutex>

class GameRoom {
public:
	GameRoom()
	{
		m_gameType = GameType::PRIVATE;
		m_roomId = 0;
		m_ownerSessionId = L"";
		m_gameName = L"";
		m_minimumLevel = 0;
		m_maximumLevel = 0;
	}

	~GameRoom()
	{

	}

	enum GameType {
		PRIVATE,
		PUBLIC
	} m_gameType;

	int32_t m_roomId;
	std::wstring m_ownerSessionId;
	std::wstring m_gameName;
	int32_t m_minimumLevel;
	int32_t m_maximumLevel;
};

typedef std::shared_ptr< GameRoom > sptr_game_room;

class LobbyServer
{
private:
	static inline std::unique_ptr< LobbyServer > m_instance;
	static inline std::mutex m_mutex;

	Timer m_timer;
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

	void Start( std::string ip, int32_t port );
	void Stop();
	bool isRunning() const
	{
		return m_running;
	}

private:
	SOCKET m_listenSocket;
	std::vector< sptr_tcp_socket > m_clientSockets;
	std::vector< uint8_t > m_recvBuffer;

	void Run();
	void CheckSocketProblem();
	void AcceptNewClient();
	void ReadSocket( sptr_tcp_socket socket );
	void WriteSocket( sptr_tcp_socket socket );
	void HandleRequest( sptr_tcp_socket socket, sptr_byte_stream stream );
};