#pragma once

#include <memory>
#include <mutex>
#include <vector>

#include "GameSession.h"

class GameSessionManager {
private:
	static inline std::unique_ptr< GameSessionManager > m_instance;
	static inline std::mutex m_mutex;
	static inline std::mutex m_dataMutex;

	int32_t m_gameIndex;
	std::vector< sptr_game_session > m_gameSessionList[ 2 ];

public:
	GameSessionManager();
	~GameSessionManager();
	GameSessionManager( const GameSessionManager & ) = delete;
	GameSessionManager &operator=( const GameSessionManager & ) = delete;

	static GameSessionManager &Get()
	{
		std::lock_guard< std::mutex > lock( m_mutex );
		if( m_instance == nullptr )
		{
			m_instance.reset( new GameSessionManager() );
		}

		return *m_instance;
	}

	void OnDisconnectUser( sptr_user user );

	bool CreatePublicGameSession( sptr_user user, const std::wstring gameName, const RealmGameType clientType );
	bool CreatePrivateGameSession( sptr_user user, const std::wstring gameName, const RealmGameType clientType );
	bool ForceTerminateGame( const int32_t gameId, RealmGameType clientType );
	sptr_game_session FindGame( const int32_t gameId, RealmGameType clientType );
	sptr_game_session FindGame( const std::wstring &gameName, RealmGameType clientType );

	bool RequestOpen( sptr_user user );
	bool RequestCancel( sptr_user user );
	bool RequestJoin( sptr_user user );
	bool RequestStart( sptr_user user );

	std::vector< sptr_game_session > GetAvailableGameSessionList( const RealmGameType clientType ) const;
	std::vector< sptr_game_session > GetPublicGameSessionList( const RealmGameType clientType ) const;
	std::vector< sptr_game_session > GetPrivateGameSessionList( const RealmGameType clientType ) const;

private:
	void ProcessJoinNorrath( sptr_user join, sptr_user host );
	void ProcessJoinArms( sptr_user join, sptr_user host );
};