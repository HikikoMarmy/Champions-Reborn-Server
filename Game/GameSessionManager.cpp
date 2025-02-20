#include "../global_define.h"
#include "GameSessionManager.h"

#include "../Lobby Server/Event/NotifyClientDiscovered.h"
#include "../Lobby Server/Event/NotifyClientReqConnect.h"
#include "../Lobby Server/Event/NotifyGameDiscovered.h"

GameSessionManager::GameSessionManager()
{
	m_gameIndex = 0;
	m_gameSessionList.clear();
}

GameSessionManager::~GameSessionManager()
{
}

void GameSessionManager::Process()
{
	for( auto &gameSession : m_gameSessionList)
	{
		ProcessGameSession( gameSession );
	}
}

void GameSessionManager::ProcessGameSession( sptr_game_session gameSession )
{
	for( auto &member : gameSession->m_userList )
	{
		if( member == nullptr )
		{
			continue;
		}
	}
}

bool GameSessionManager::CreatePublicGameSession( sptr_user owner, std::wstring gameName, int32_t minimumLevel, int32_t maximumLevel )
{
	auto new_session = std::make_shared< GameSession >();

	new_session->m_type = GameSession::GameType::Public;
	new_session->m_gameIndex = m_gameIndex;
	new_session->m_gameLocation = L"Kelethin";
	new_session->m_gameName = gameName;
	new_session->m_minimumLevel = minimumLevel;
	new_session->m_maximumLevel = maximumLevel;

	new_session->m_gameData.resize(256);

	owner->is_host = true;
	owner->is_ready = false;
	owner->discovery_addr = "";
	owner->discovery_port = 0;
	owner->discovery_state = DiscoveryState::Initial_Connect;

	owner->game_id = m_gameIndex;
	new_session->m_userList.push_back(owner);

	m_gameSessionList.push_back( new_session );

	m_gameIndex++;

	return true;
}

bool GameSessionManager::CreatePrivateGameSession( sptr_user owner, std::wstring gameName, int32_t minimumLevel, int32_t maximumLevel )
{
	// Check if the game name or host session id is already in use
	for( auto &gameSession : m_gameSessionList )
	{
		if( gameSession->m_type != GameSession::GameType::Private )
			continue;
		
		if( gameSession->m_gameName == gameName )
		{
			Log::Error( "Game name is already in use! [%S]", gameName.c_str() );
			return false;
		}
	}

	auto new_session = std::make_shared< GameSession >();

	new_session->m_type = GameSession::GameType::Private;
	new_session->m_gameIndex = m_gameIndex;
	new_session->m_gameLocation = L"Kelethin";
	new_session->m_gameName = gameName;
	new_session->m_minimumLevel = minimumLevel;
	new_session->m_maximumLevel = maximumLevel;

	new_session->m_gameData.resize( 256 );

	owner->is_host = true;
	owner->is_ready = false;
	owner->discovery_addr = "";
	owner->discovery_port = 0;
	owner->discovery_state = DiscoveryState::Initial_Connect;

	owner->game_id = m_gameIndex;
	new_session->m_userList.push_back( owner );

	m_gameSessionList.push_back( new_session );

	m_gameIndex++;

	return true;
}

bool GameSessionManager::CancelGameSession( sptr_user user )
{
	auto gameId = user->game_id;

	if( gameId < 0 )
	{
		Log::Error( "Game session not found! [%d]", gameId );
		return false;
	}

	auto it = std::find_if( m_gameSessionList.begin(), m_gameSessionList.end(), [ &gameId ]( sptr_game_session gameSession )
	{
		return gameSession->m_gameIndex == gameId;
	} );

	Log::Debug( "CancelGameSession : [%d]", gameId );
	Log::Debug( "TODO: Notify all users in the game session" );

	if( it != m_gameSessionList.end() )
	{
		m_gameSessionList.erase( it );
		return true;
	}

	return false;
}

sptr_game_session GameSessionManager::FindGame( const int32_t gameId )
{
	if (gameId < 0) return nullptr;

	for( auto &gameSession : m_gameSessionList)
	{
		if( gameSession->m_gameIndex == gameId )
		{
			return gameSession;
		}
	}

	return nullptr;
}

bool GameSessionManager::SetGameOpen(sptr_user user)
{
	auto gameId = user->game_id;
	auto session = FindGame(gameId);

	if (session == nullptr)
	{
		Log::Error("Game session not found! [%d]", gameId);
		return false;
	}

	if( session->m_state != GameSession::GameState::NotReady )
	{
		return false;
	}

	session->m_state = GameSession::GameState::Open;

	user->is_ready = true;

	Log::Info("Game session is open! [%d]", gameId);

	return true;
}

bool GameSessionManager::JoinGame(sptr_user user)
{
	auto gameId = user->game_id;
	auto session = FindGame(gameId);

	if (session == nullptr)
	{
		Log::Error("Game session not found! [%d]", gameId);
		return false;
	}

	auto host = session->GetHost();

	// Check that the user isn't already in the list.
	auto it = std::find_if(session->m_userList.begin(), session->m_userList.end(), [&user](sptr_user member)
	{
		return member == user;
	});

	if (it != session->m_userList.end())
	{
		Log::Error("User is already in the game session! [%d]", gameId);
		return false;
	}

	// Add the user to the game session.
	user->is_host = false;
	user->is_ready = false;
	session->m_userList.push_back(user);

	// Notify the host that a user is joining.
	NotifyClientRequestConnect msg_a( user->discovery_addr, user->discovery_port );
	host->tcp->send( msg_a );

	// Update the user with their discovery address.
	NotifyGameDiscovered msg_b( user->discovery_addr, user->discovery_port );
	user->tcp->send( msg_b );

	Log::Info("User [%S] joined game session! [%d]", user->session_id.c_str(), gameId);

	return true;
}

void GameSessionManager::RemoveUser(sptr_user user)
{
	if( user == nullptr )
	{
		Log::Error( "User is null!" );
		return;
	}

	auto session = FindGame( user->game_id );

	if( session == nullptr )
	{
		Log::Error( "Game session not found! [%d]", user->game_id );
		return;
	}

	auto it = std::find_if( session->m_userList.begin(), session->m_userList.end(), [ &user ]( sptr_user member )
	{
		return member == user;
	} );

	if( it != session->m_userList.end() )
	{
		session->m_userList.erase( it );
	}

	if( session->m_userList.empty() )
	{
		
	}
}

std::vector<sptr_game_session> GameSessionManager::GetPublicGameSessionList() const
{
	std::vector< sptr_game_session > list;

	for( auto &game : m_gameSessionList )
	{
		if( game->m_type == GameSession::GameType::Public )
		{
			list.push_back( game );
		}
	}

	return list;
}

std::vector<sptr_game_session> GameSessionManager::GetAvailableGameSessionList() const
{
	std::vector<sptr_game_session> list;

	for( auto &game : m_gameSessionList)
	{
		if (game->m_type != GameSession::GameType::Public)
			continue;

		//if( game->m_state != GameSession::GameState::Open)
		//	continue;

		list.push_back(game);
	}

	return list;
}

std::vector<sptr_game_session> GameSessionManager::GetPrivateGameSessionList() const
{
	std::vector<sptr_game_session> list;

	for( auto &game : m_gameSessionList )
	{
		if( game->m_type == GameSession::GameType::Private )
		{
			list.push_back( game );
		}
	}

	return list;
}