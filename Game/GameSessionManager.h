#pragma once

#include "GameSession.h"

class GameSessionManager {
private:
	static inline std::unique_ptr< GameSessionManager > m_instance;
	static inline std::mutex m_mutex;

	int32_t m_gameIndex;
	std::vector< sptr_game_session > m_gameSessionList;

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

	bool CreatePublicGameSession( sptr_user user, std::wstring gameName, int32_t minimumLevel, int32_t maximumLevel );
	bool CreatePrivateGameSession( sptr_user user, std::wstring gameName, int32_t minimumLevel, int32_t maximumLevel );
	bool CancelGameSession( sptr_user user );

	sptr_game_session FindGame( const int32_t gameId );

	bool SetGameOpen( sptr_user user );
	bool JoinGame( sptr_user user );
	void RemoveUser( sptr_user user );

	std::vector< sptr_game_session > GetPublicGameSessionList() const;
	std::vector< sptr_game_session > GetAvailableGameSessionList() const;
	std::vector< sptr_game_session > GetPrivateGameSessionList() const;
};