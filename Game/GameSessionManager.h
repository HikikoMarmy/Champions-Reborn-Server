#pragma once

#include "GameSession.h"

class GameSessionManager {
private:
	static inline std::unique_ptr< GameSessionManager > m_instance;
	static inline std::mutex m_mutex;

	int32_t m_gameIndex;
	std::vector< sptr_game_session > m_publicGameSessionList;
	std::vector< sptr_game_session > m_privateGameSessionList;

public:
	GameSessionManager();
	~GameSessionManager();
	GameSessionManager( const GameSessionManager & ) = delete;
	GameSessionManager &operator=( const GameSessionManager & ) = delete;

	static GameSessionManager& Get()
	{
		std::lock_guard< std::mutex > lock( m_mutex );
		if( m_instance == nullptr )
		{
			m_instance.reset( new GameSessionManager() );
		}

		return *m_instance;
	}

	void Process();
	void ProcessGameSession( sptr_game_session gameSession );

	bool CreatePublicGameSession( sptr_user owner, std::wstring gameName, int32_t minimumLevel, int32_t maximumLevel );
	bool CreatePrivateGameSession( sptr_user owner, std::wstring gameName, int32_t minimumLevel, int32_t maximumLevel );
	bool CancelGameSession( std::wstring sessionId );

	sptr_game_session FindGame( const std::wstring sessionId );
	sptr_game_session FindGame( const int32_t gameId );

	bool UserJoinGame( const int32_t gameId, sptr_user joiningUser );

private:
	void HandleJoinDiscovery( sptr_game_session gameSession, sptr_user joiningUser );
};