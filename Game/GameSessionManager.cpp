#include "../global_define.h"
#include "GameSessionManager.h"

GameSessionManager::GameSessionManager()
{
	m_gameIndex = 0;
	m_publicGameSessionList.clear();
	m_privateGameSessionList.clear();
}

GameSessionManager::~GameSessionManager()
{
}

bool GameSessionManager::CreatePublicGameSession( sptr_user owner, std::wstring gameName, int32_t minimumLevel, int32_t maximumLevel )
{
	auto &hostSessionId = owner->m_sessionId;

	// Check if the host session id is already in use
	for( auto &gameSession : m_publicGameSessionList )
	{
		if( gameSession->m_hostSessionId == hostSessionId )
		{
			Log::Error( "Host session id is already in use! [%S]", hostSessionId.c_str() );
			return false;
		}
	}

	auto new_session = std::make_shared< GameSession >();

	new_session->m_gameIndex = m_gameIndex++;
	new_session->m_hostSessionId = hostSessionId;
	new_session->m_gameName = gameName;
	new_session->m_minimumLevel = minimumLevel;
	new_session->m_maximumLevel = maximumLevel;

	new_session->m_userList[ 0 ] = owner;

	m_publicGameSessionList.push_back( new_session );

	return true;
}

bool GameSessionManager::CreatePrivateGameSession( sptr_user owner, std::wstring gameName, int32_t minimumLevel, int32_t maximumLevel )
{
	auto &hostSessionId = owner->m_sessionId;

	// Check if the game name or host session id is already in use
	for( auto &gameSession : m_privateGameSessionList )
	{
		if( gameSession->m_hostSessionId == hostSessionId )
		{
			Log::Error( "Host session id is already in use! [%S]", hostSessionId.c_str() );
			return false;
		}

		if( gameSession->m_gameName == gameName )
		{
			Log::Error( "Game name is already in use! [%S]", gameName.c_str() );
			return false;
		}
	}

	auto new_session = std::make_shared< GameSession >();

	new_session->m_gameIndex = -1;
	new_session->m_hostSessionId = hostSessionId;
	new_session->m_gameName = gameName;
	new_session->m_minimumLevel = minimumLevel;
	new_session->m_maximumLevel = maximumLevel;

	m_privateGameSessionList.push_back( new_session );

	return false;
}

bool GameSessionManager::UpdateGameSessionDiscovery( sptr_user owner, std::string hostIp, int32_t hostPort )
{
	auto &hostSessionId = owner->m_sessionId;

	for( auto &gameSession : m_publicGameSessionList )
	{
		if( gameSession->m_hostSessionId == hostSessionId )
		{
			//gameSession->m_hostIp = hostIp;
			//gameSession->m_hostPort = hostPort;

			return true;
		}
	}

	for( auto &gameSession : m_privateGameSessionList )
	{
		if( gameSession->m_hostSessionId == hostSessionId )
		{
			//gameSession->m_hostIp = hostIp;
			//gameSession->m_hostPort = hostPort;

			return true;
		}
	}

	Log::Error( "Failed to update game session discovery information! [%S]", hostSessionId.c_str() );

	return false;
}

sptr_game_session GameSessionManager::FindGame( const std::wstring sessionId )
{
	for( auto &gameSession : m_publicGameSessionList )
	{
		if( gameSession->m_hostSessionId == sessionId )
		{
			return gameSession;
		}
	}

	for( auto &gameSession : m_privateGameSessionList )
	{
		if( gameSession->m_hostSessionId == sessionId )
		{
			return gameSession;
		}
	}

	return nullptr;
}

sptr_game_session GameSessionManager::FindGame( const int32_t gameId )
{
	for( auto &gameSession : m_publicGameSessionList )
	{
		if( gameSession->m_gameIndex == gameId )
		{
			return gameSession;
		}
	}

	return nullptr;
}

bool GameSessionManager::UserJoinGame( const int32_t gameId, sptr_user joiningUser )
{
	auto gameSession = FindGame( gameId );

	if( gameSession == nullptr )
	{
		Log::Error( "Game session not found! [%d]", gameId );
		return false;
	}

	for( auto &user : gameSession->m_userList )
	{
		if( user == nullptr )
		{
			user = joiningUser;
			user->m_state = RealmUser::UserState::JoinPending;
			return true;
		}
	}

	Log::Error( "Game session is full! [%d]", gameId );

	return false;
}
