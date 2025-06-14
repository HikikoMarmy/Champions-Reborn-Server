#include "GameSessionManager.h"

#include "RealmUser.h"
#include "../Network/Event/NotifyClientDiscovered.h"
#include "../Network/Event/NotifyClientDiscovered_RTA.h"
#include "../Network/Event/NotifyClientRequestConnect.h"
#include "../Network/Event/NotifyClientRequestConnect_RTA.h"
#include "../Network/Event/NotifyGameDiscovered.h"
#include "../Network/Event/NotifyReserveUserSlot_RTA.h"
#include "../../logging.h"

GameSessionManager::GameSessionManager()
{
	m_gameIndex = 0;
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

	auto owner = session->m_owner.lock();
	if( !owner )
	{
		Log::Error( "Game session owner not found! [%d]", gameId );
		ForceTerminateGame( gameId, gameType );
		return;
	}

	if( owner->m_sessionId == user->m_sessionId )
	{
		Log::Info( "Game session owner disconnected! [%d]", gameId );
		ForceTerminateGame( gameId, gameType );
	}
}

bool GameSessionManager::CreatePublicGameSession( sptr_user owner, std::wstring gameName, RealmGameType clientType )
{
	auto new_session = std::make_shared< GameSession >();

	new_session->m_type = GameSession::GameType::Public;
	new_session->m_gameIndex = m_gameIndex;
	new_session->m_hostLocalAddr.clear();
	new_session->m_hostExternalAddr.clear();
	new_session->m_gameName = gameName;
	new_session->m_currentPlayers = 1;
	new_session->m_maximumPlayers = 4;

	new_session->m_gameData.resize( 256 );

	owner->m_isHost = true;
	owner->m_discoveryAddr = "";

	owner->m_gameId = m_gameIndex;
	new_session->m_owner = owner;

	std::lock_guard< std::mutex > lock( m_dataMutex );
	m_gameSessionList[ clientType ].push_back( new_session );

	m_gameIndex++;

	return true;
}

bool GameSessionManager::CreatePrivateGameSession( sptr_user owner, std::wstring gameName, RealmGameType clientType )
{
	// Check if the game name or host session id is already in use
	for( const auto &gameSession : m_gameSessionList[ clientType ] )
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
	new_session->m_hostLocalAddr.clear();
	new_session->m_hostExternalAddr.clear();
	new_session->m_gameName = gameName;
	new_session->m_currentPlayers = 1;
	new_session->m_maximumPlayers = 4;

	new_session->m_gameData.resize( 256 );

	owner->m_isHost = true;
	owner->m_discoveryAddr = "";

	owner->m_gameId = m_gameIndex;
	new_session->m_owner = owner;

	std::lock_guard< std::mutex > lock( m_dataMutex );
	m_gameSessionList[ clientType ].push_back( new_session );

	m_gameIndex++;

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
		return gameSession->m_gameIndex == gameId;
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
		if( gameSession->m_gameIndex == gameId )
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
		Log::Error( "Game session not found! [%d]", gameId );
		return false;
	}

	if( session->m_state == GameSession::GameState::Open )
	{
		return false;
	}

	if( user->m_discoveryAddr.empty() )
	{
		Log::Error( "User discovery address is empty! [%d]", gameId );
		return false;
	}

	session->m_hostExternalAddr = user->m_discoveryAddr;
	session->m_hostLocalAddr = user->m_localAddr;
	session->m_hostPort = user->m_discoveryPort;

	session->m_state = GameSession::GameState::Open;

	// Tell the host its own address.
	NotifyGameDiscovered msg( user->m_discoveryAddr, user->m_discoveryPort, user->m_gameType );
	user->sock->send( msg );

	Log::Info( "Game Session [%d] Discoverable on %s", gameId, user->m_discoveryAddr.c_str() );

	return true;
}

// NOTE:
// This might solely be for a user that is LEAVING the game (I.E "CancelJoining")
// instead of a game being cancelled overall. RTA has a dedicated endgame event.
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
		return gameSession->m_gameIndex == gameId;
	} );

	if( it == gameList.end() )
		return false;

	const auto &session = *it;

	auto owner = session->m_owner.lock();
	if( !owner )
	{
		Log::Error( "Game session owner not found! [%d]", gameId );
		ForceTerminateGame( gameId, gameType );
		return false;
	}

	if( owner->m_sessionId != user->m_sessionId )
	{
		Log::Error( "User is not the host! [%d]", gameId );
		return false;
	}

	gameList.erase( it );
	return true;
}

bool GameSessionManager::RequestJoin( sptr_user join_user )
{
	const auto gameId = join_user->m_gameId;
	const auto gameType = join_user->m_gameType;
	auto session = FindGame( gameId, gameType );

	if( session == nullptr )
	{
		Log::Error( "Game session not found! [%d]", gameId );
		return false;
	}

	if( session->m_state != GameSession::GameState::Open )
	{
		Log::Error( "Game session not open! [%d]", gameId );
		return false;
	}

	auto host_user = session->m_owner.lock();

	if( host_user == nullptr )
	{
		Log::Error( "Host not found! [%d]", gameId );
		ForceTerminateGame( gameId, gameType );
		return false;
	}

	if( host_user->m_discoveryAddr.empty() )
	{
		Log::Error( "User discovery address is empty! [%d]", gameId );
		ForceTerminateGame( gameId, gameType );
		return false;
	}

	join_user->m_isHost = false;

	if( host_user->m_gameType == RealmGameType::CHAMPIONS_OF_NORRATH )
	{
		ProcessJoinNorrath( join_user, host_user );
	}
	else
	{
		ProcessJoinArms( join_user, host_user );
	}

	Log::Info( "User [%S] Joining game session... [%d]", join_user->m_sessionId.c_str(), gameId );

	return true;
}

// NOTE:
// Request Start seems to be for RTA only.
// CON will disconnect from the server at start time.
bool GameSessionManager::RequestStart(sptr_user user)
{
	const auto gameId = user->m_gameId;
	const auto gameType = user->m_gameType;	

	auto session = FindGame( gameId, gameType );

	if( session == nullptr )
	{
		Log::Error( "Game session not found! [%d]", gameId );
		return false;
	}

	std::lock_guard<std::mutex> lock( m_dataMutex );

	session->m_state = GameSession::GameState::Started;

	// Temp (or permanent) remove the game from the list.
	auto &gameList = m_gameSessionList[ gameType ];

	const auto it = std::find_if( gameList.begin(), gameList.end(),
								  [ gameId ]( const sptr_game_session &gameSession )
	{
		return gameSession->m_gameIndex == gameId;
	} );

	if( it != gameList.end() )
	{
		gameList.erase( it );
	}

	Log::Info( "Game session [%d] started", gameId );

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

void GameSessionManager::ProcessJoinNorrath(sptr_user join, sptr_user host)
{
	std::string hostAddr = host->m_discoveryAddr;
	std::string joinAddr = join->m_discoveryAddr;

	if (hostAddr == joinAddr)
	{
		hostAddr = host->m_localAddr;
		// I don't think the joiner ever reports its local address for CON.
		// At best, we can report the hosts local IP and hope communication works.
	}

	// Tell the joining user the hosts address.
	NotifyClientDiscovered msgClientDiscovered( hostAddr, host->m_discoveryPort );
	join->sock->send(msgClientDiscovered);

	// Tell the host the joining user's address.
	NotifyClientRequestConnect msgNotifyReqConnect(
		joinAddr,
		join->m_discoveryPort
	);

	host->sock->send(msgNotifyReqConnect);
}

void GameSessionManager::ProcessJoinArms(sptr_user join, sptr_user host)
{
	std::string hostAddr = host->m_discoveryAddr;
	std::string joinAddr = join->m_discoveryAddr;

	if (hostAddr == joinAddr)
	{
		hostAddr = host->m_localAddr;
		joinAddr = join->m_localAddr;
	}

	// Tell the joining user the hosts address.
	NotifyClientDiscovered_RTA msgClientDiscovered(hostAddr, host->m_discoveryPort);
	join->sock->send(msgClientDiscovered);

	// Reserve a slot for the joining user.
	NotifyReserveUserSlot_RTA msgNotifyReserveUser(joinAddr, join->m_discoveryPort);
	host->sock->send(msgNotifyReserveUser);

	// Tell the host the joining user's address.
	NotifyClientRequestConnect_RTA msgNotifyReqConnect(
		joinAddr,
		join->m_discoveryAddr,
		join->m_discoveryPort
	);

	host->sock->send(msgNotifyReqConnect);
}
