#include "../global_define.h"
#include "GameSessionManager.h"

#include "../Lobby Server/Event/NotifyClientDiscovered.h"
#include "../Lobby Server/Event/NotifyClientReqConnect.h"
#include "../Lobby Server/Event/NotifyGameDiscovered.h"

GameSessionManager::GameSessionManager()
{
	m_gameIndex = 0;
	m_publicGameSessionList.clear();
	m_privateGameSessionList.clear();
}

GameSessionManager::~GameSessionManager()
{
}

void GameSessionManager::Process()
{
	for( auto &gameSession : m_publicGameSessionList )
	{
		ProcessGameSession( gameSession );
	}

	for( auto &gameSession : m_privateGameSessionList )
	{
		ProcessGameSession( gameSession );
	}
}

void GameSessionManager::ProcessGameSession( sptr_game_session gameSession )
{
	static int test = 0;

	//if( 0 == onePass ) return;

	for( auto &member : gameSession->m_userList )
	{
		if( member == nullptr )
		{
			continue;
		}

		if( member->m_state == RealmUser::UserState::DiscoveryPending )
		{
			continue;
		}

		//if( member->m_state == RealmUser::UserState::JoinPending )
		if( test == 1 )
		{
			//HandleJoinDiscovery( gameSession, member );
			

			//auto gameOwner = gameSession->GetOwner();

			//NotifyClientRequestConnect notify( discoveryInfo->m_ip.c_str(), discoveryInfo->m_port );
			//gameOwner->m_realmSocket->send( notify );
			//
			//member->m_state = RealmUser::UserState::InGameLobby;
			//
			//NotifyGameDiscovered notifyGameDiscover( discoveryInfo->m_ip.c_str(), discoveryInfo->m_port );
			//member->m_realmSocket->send( notifyGameDiscover );
			

			
			

			// Notify the host that a user has joined the game
			//NotifyClientRequestConnect notify( "192.168.1.248", 47115 );
			
			//NotifyClientRequestConnect notify( "192.168.1.248", 40820 );
			//gameOwner->m_realmSocket->send( notify );

			// Notify the user that the host has accepted the connection
			//NotifyGameDiscovered notify2( gameOwner->discovery.ip, gameOwner->discovery.port );
			//member->m_realmSocket->send( notify2 );
		}


		if( test == 2 )
		{
			//NotifyGameDiscovered msg2( "192.168.1.248", 3001 );
			//member->m_realmSocket->send( msg2 );

			//NotifyClientDiscovered msg1( "192.168.1.248", 3001 );
			//member->m_realmSocket->send( msg1 );

			NotifyClientRequestConnect notify( "127.0.0.1", 3001 );
			member->m_realmSocket->send( notify );
		}
	}

	test = 0;
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

bool GameSessionManager::CancelGameSession( std::wstring sessionId )
{
	for( auto it = m_publicGameSessionList.begin(); it != m_publicGameSessionList.end(); ++it )
	{
		if( ( *it )->m_hostSessionId == sessionId )
		{
			Log::Debug( "Public Game session canceled! [%S]", sessionId.c_str() );
			m_publicGameSessionList.erase( it );
			return true;
		}
	}

	for( auto it = m_privateGameSessionList.begin(); it != m_privateGameSessionList.end(); ++it )
	{
		if( ( *it )->m_hostSessionId == sessionId )
		{
			Log::Debug( "Private Game session canceled! [%S]", sessionId.c_str() );
			m_privateGameSessionList.erase( it );
			return true;
		}
	}

	Log::Error( "Failed to cancel game session! [%S]", sessionId.c_str() );
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

void GameSessionManager::HandleJoinDiscovery( sptr_game_session gameSession, sptr_user joiningUser )
{
	auto discoveryInfo = DiscoveryServer::Get().GetDiscoveryInfo( joiningUser->m_sessionId );

	if( !discoveryInfo.has_value() )
	{
		Log::Error( "Discovery info not found! [%S]", joiningUser->m_sessionId.c_str() );
		return;
	}

	Log::Debug( "Member Discovery: %s:%d", discoveryInfo->m_ip.c_str(), discoveryInfo->m_port );

	auto hostUser = gameSession->GetOwner();

	NotifyClientRequestConnect notify( "192.168.1.248", 3001 );
	hostUser->m_realmSocket->send( notify );
}
