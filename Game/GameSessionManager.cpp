#include "GameSessionManager.h"

#include "RealmUser.h"
#include "../Network/Event/NotifyGameDiscovered.h"
#include "../Network/Event/NotifyClientDiscovered.h"
#include "../Network/Event/NotifyClientDiscovered_RTA.h"
#include "../Network/Event/NotifyClientRequestConnect.h"
#include "../Network/Event/NotifyClientRequestConnect_RTA.h"
#include "../../logging.h"

GameSessionManager::GameSessionManager()
{
	m_uniqueGameIndex = 0;
	m_gameSessionList[ 0 ].clear();
	m_gameSessionList[ 1 ].clear();
}

GameSessionManager::~GameSessionManager()
{
}

void GameSessionManager::OnDisconnectUser( sptr_user user )
{
	if( !user || user->m_gameId < 0 )
		return;

	const auto gameId = user->m_gameId;
	const auto gameType = user->m_gameType;

	auto session = FindGame( gameId, gameType );
	if( !session )
		return;

	const auto owner = session->GetOwner();
	if( !owner )
	{
		Log::Error( "Game session owner not found! [{}]", gameId );
		ForceTerminateGame( gameId, gameType );
		return;
	}

	if( owner->m_sessionId == user->m_sessionId )
	{
		Log::Info( "Game session owner disconnected! [{}]", gameId );
		ForceTerminateGame( gameId, gameType );
	}
}

bool GameSessionManager::CreateGameSession_CON( sptr_user user,
												const std::wstring gameInfo,
												const std::wstring name,
												const std::wstring stage,
												bool isPrivateGame )
{
	if( name.empty() )
	{
		Log::Error( "Invalid parameters for creating game session!" );
		return false;
	}

	auto new_session = std::make_shared< GameSession >( m_uniqueGameIndex++ );

	if( isPrivateGame )
	{
		new_session->m_type = GameSession::GameType::Private;
		new_session->m_gameName = name;
	}
	else
	{
		new_session->m_type = GameSession::GameType::Public;
		new_session->m_gameName = name + L" [" + stage + L"]";
	}

	user->m_isHost = true;
	user->m_gameId = new_session->m_gameId;
	user->m_discoveryAddr = "";
	user->m_discoveryPort = 0;

	new_session->AddMember( user );

	std::lock_guard< std::mutex > lock( m_dataMutex );
	m_gameSessionList[ RealmGameType::CHAMPIONS_OF_NORRATH ].push_back( new_session );

	return true;
}

bool GameSessionManager::CreateGameSession_RTA(
	sptr_user user,
	const std::wstring gameInfo,
	const std::wstring name,
	const std::array< int8_t, 5 > &attributes,
	const bool isPrivateGame )
{
	if( nullptr != FindGame( name, RealmGameType::RETURN_TO_ARMS ) )
	{
		Log::Error( "Game name is already in use! [{}]", name );
		return false;
	}

	auto new_session = std::make_shared< GameSession >( m_uniqueGameIndex++ );

	if( isPrivateGame )
	{
		new_session->m_type = GameSession::GameType::Private;
	}
	else
	{
		new_session->m_type = GameSession::GameType::Public;

		new_session->m_gameData = Util::WideToUTF8( gameInfo );
		new_session->m_difficulty = attributes[ 0 ];
		new_session->m_gameMode = attributes[ 1 ];
		new_session->m_unknown = attributes[ 2 ];
		new_session->m_mission = attributes[ 3 ];
		new_session->m_networkSave = attributes[ 4 ];
	}

	new_session->m_gameName = name;

	user->m_isHost = true;
	user->m_gameId = new_session->m_gameId;
	user->m_discoveryAddr = "";
	user->m_discoveryPort = 0;

	new_session->AddMember( user );

	std::lock_guard< std::mutex > lock( m_dataMutex );
	m_gameSessionList[ RealmGameType::RETURN_TO_ARMS ].push_back( new_session );

	return true;
}

bool GameSessionManager::ForceTerminateGame( int32_t gameId, RealmGameType clientType )
{
	if( gameId < 0 )
	{
		return false;
	}

	std::lock_guard< std::mutex > lock( m_dataMutex );

	const auto &gameList = m_gameSessionList[ clientType ];
	const auto it = std::find_if( gameList.begin(), gameList.end(), [ &gameId ]( sptr_game_session gameSession )
	{
		return gameSession->m_gameId == gameId;
	} );

	if( it != m_gameSessionList[ clientType ].end() )
	{
		m_gameSessionList[ clientType ].erase( it );
		return true;
	}

	return false;
}

sptr_game_session GameSessionManager::FindGame( const int32_t gameId, const RealmGameType gameType )
{
	if( gameId < 0 ) return nullptr;

	std::lock_guard< std::mutex > lock( m_dataMutex );

	for( auto &gameSession : m_gameSessionList[ gameType ] )
	{
		if( gameSession->m_gameId == gameId )
		{
			return gameSession;
		}
	}

	return nullptr;
}

sptr_game_session GameSessionManager::FindGame( const std::wstring &gameName, const RealmGameType gameType )
{
	if( gameName.empty() ) return nullptr;

	std::lock_guard< std::mutex > lock( m_dataMutex );

	for( auto &gameSession : m_gameSessionList[ gameType ] )
	{
		if( gameSession->m_gameName == gameName )
		{
			return gameSession;
		}
	}

	return nullptr;
}

bool GameSessionManager::RequestOpen( sptr_user user )
{
	auto gameId = user->m_gameId;
	auto gameType = user->m_gameType;
	auto session = FindGame( gameId, gameType );

	if( session == nullptr )
	{
		Log::Error( "Game session not found! [{}]", gameId );
		return false;
	}

	if( session->m_state == GameSession::GameState::Open )
	{
		return false;
	}

	if( user->m_discoveryAddr.empty() )
	{
		Log::Error( "User discovery address is empty! [{}]", gameId );
		return false;
	}

	session->m_hostLocalAddr = user->m_localAddr;
	session->m_hostLocalPort = user->m_localPort;

	session->m_hostExternalAddr = user->m_discoveryAddr;
	session->m_hostNatPort = user->m_discoveryPort;

	session->m_state = GameSession::GameState::Open;

	// Tell the host its own address.
	user->sock->send( NotifyGameDiscovered( user ) );

	Log::Info( "Game Session [{}] Discoverable on {}", gameId, user->m_discoveryAddr );

	return true;
}

bool GameSessionManager::RequestCancel( sptr_user user )
{
	if( !user || user->m_gameId < 0 )
		return false;

	std::lock_guard<std::mutex> lock( m_dataMutex );

	const auto gameId = user->m_gameId;
	const auto gameType = user->m_gameType;
	auto &gameList = m_gameSessionList[ gameType ];

	const auto it = std::find_if( gameList.begin(), gameList.end(),
								  [ gameId ]( const sptr_game_session &gameSession )
	{
		return gameSession->m_gameId == gameId;
	} );

	if( it == gameList.end() )
		return false;

	const auto &session = *it;

	if( false == session->RemoveMember( user ) )
	{
		Log::Error( "Failed to remove user [{}] from game session [{}]", user->m_username, gameId );
	}

	if( session->m_currentPlayers <= 0 )
	{
		Log::Info( "Game session [{}] is empty, removing it", gameId );
		gameList.erase( it );
	}
	else
	{
		Log::Info( "User [{}] left game session [{}], remaining players: {}",
				   user->m_username, gameId, session->m_currentPlayers );
	}

	return true;
}

bool GameSessionManager::RequestJoin( sptr_user join_user )
{
	const auto gameId = join_user->m_gameId;
	const auto gameType = join_user->m_gameType;
	auto session = FindGame( gameId, gameType );

	if( session == nullptr )
	{
		Log::Error( "Game session not found! [{}]", gameId );
		return false;
	}

	if( session->m_state != GameSession::GameState::Open )
	{
		Log::Error( "Game session not open! [{}]", gameId );
		return false;
	}

	auto host_user = session->GetOwner();

	if( host_user == nullptr )
	{
		Log::Error( "Host not found! [{}]", gameId );
		ForceTerminateGame( gameId, gameType );
		return false;
	}

	if( host_user->m_discoveryAddr.empty() )
	{
		Log::Error( "User discovery address is empty! [{}]", gameId );
		ForceTerminateGame( gameId, gameType );
		return false;
	}

	if( host_user->m_gameType == RealmGameType::CHAMPIONS_OF_NORRATH )
	{
		ProcessJoinNorrath( join_user, host_user );
	}
	else
	{
		ProcessJoinArms( join_user, host_user );
	}

	Log::Info( "User [{}] Joining game session... [{}]", join_user->m_sessionId, gameId );

	return true;
}

// NOTE:
// Request Start seems to be for RTA only.
// CON will disconnect from the server at start time.
bool GameSessionManager::RequestStart( sptr_user user )
{
	const auto gameId = user->m_gameId;
	const auto gameType = user->m_gameType;

	auto session = FindGame( gameId, gameType );

	if( session == nullptr )
	{
		Log::Error( "Game session not found! [{}]", gameId );
		return false;
	}

	std::lock_guard<std::mutex> lock( m_dataMutex );

	session->m_state = GameSession::GameState::Started;

	// Remove the game from the list.
	auto &gameList = m_gameSessionList[ gameType ];

	const auto it = std::find_if( gameList.begin(), gameList.end(),
								  [ gameId ]( const sptr_game_session &gameSession )
	{
		return gameSession->m_gameId == gameId;
	} );

	if( it != gameList.end() )
	{
		gameList.erase( it );
	}

	Log::Info( "Game session [{}] started", gameId );

	return true;
}

std::vector<sptr_game_session> GameSessionManager::GetAvailableGameSessionList( const RealmGameType gameType ) const
{
	std::lock_guard<std::mutex> lock( m_dataMutex );

	std::vector<sptr_game_session> list;
	for( const auto &game : m_gameSessionList[ gameType ] )
	{
		if( game->m_type == GameSession::GameType::Public &&
			game->m_state == GameSession::GameState::Open )
		{
			list.push_back( game );
		}
	}
	return list;
}

std::vector<sptr_game_session> GameSessionManager::GetPublicGameSessionList( const RealmGameType gameType ) const
{
	std::lock_guard<std::mutex> lock( m_dataMutex );

	std::vector<sptr_game_session> list;
	for( const auto &game : m_gameSessionList[ gameType ] )
	{
		if( game->m_type == GameSession::GameType::Public )
			list.push_back( game );
	}
	return list;
}

std::vector<sptr_game_session> GameSessionManager::GetPrivateGameSessionList( const RealmGameType gameType ) const
{
	std::lock_guard<std::mutex> lock( m_dataMutex );

	std::vector<sptr_game_session> list;
	for( const auto &game : m_gameSessionList[ gameType ] )
	{
		if( game->m_type == GameSession::GameType::Private )
			list.push_back( game );
	}
	return list;
}

void GameSessionManager::ProcessJoinNorrath( sptr_user join, sptr_user host )
{
	// Tell the joining user its own address
	join->sock->send( NotifyClientDiscovered( join ) );

	// Tell the host the joining user's address.
	host->sock->send( NotifyClientRequestConnect( join ) );
}

void GameSessionManager::ProcessJoinArms( sptr_user join, sptr_user host )
{
	Log::Debug("Join User IPs : [{}:{}] -> [{}:{}]",
		join->m_localAddr, join->m_localPort, join->m_discoveryAddr, join->m_discoveryPort );

	Log::Debug("Host User IPs : [{}:{}] -> [{}:{}]",
		host->m_localAddr, host->m_localPort, host->m_discoveryAddr, host->m_discoveryPort );

	// Tell the joining user its own address
	join->sock->send( NotifyClientDiscovered_RTA( join ) );

	// Tell the host the joining user's address.
	host->sock->send( NotifyClientRequestConnect_RTA( join ) );
}
