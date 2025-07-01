#include "RealmUserManager.h"
#include "GameSessionManager.h"
#include "ChatRoomManager.h"

#include "../Network/Event/NotifyForcedLogout.h"
#include "../Common/Constant.h"
#include "../Database/Database.h"
#include "../logging.h"

UserManager::UserManager()
{
	std::random_device rd;
	rng.seed( rd() );

	m_users.clear();
}

UserManager::~UserManager()
{
}

std::wstring UserManager::GenerateSessionId()
{
	static const wchar_t charset[] = L"0123456789ABCDEF";
	std::uniform_int_distribution<int> dist( 0, 15 );

	std::wstring sessionId;
	sessionId.reserve( MAX_SESSION_ID_LENGTH );

	for( int i = 0; i < MAX_SESSION_ID_LENGTH; ++i )
	{
		sessionId += charset[ dist( rng ) ];
	}

	return sessionId;
}

sptr_user UserManager::CreateUser( sptr_socket socket, RealmGameType clientType )
{
	Log::Debug( "ClientManager::CreateUser() - Created new user" );

	auto user = std::make_shared< RealmUser >();

	user->sock = socket;
	user->m_gameType = clientType;

	std::lock_guard< std::mutex > lock( m_mutex );
	m_users.push_back( user );

	return user;
}

void UserManager::RemoveUser( sptr_user user )
{
	auto it = std::find( m_users.begin(), m_users.end(), user );
	if( it == m_users.end() )
	{
		Log::Error( "RemoveUser : [{}] not found", user->m_sessionId );
		return;
	}

	Database::Get().DeleteSession( user->m_sessionId );
	GameSessionManager::Get().OnDisconnectUser( user );
	ChatRoomManager::Get().OnDisconnectUser( user );

	Log::Debug( "RemoveUser : [{}][{}]", user->m_username, user->m_sessionId );

	std::lock_guard< std::mutex > lock( m_mutex );
	m_users.erase( it );
}

void UserManager::RemoveUser( const std::wstring &sessionId )
{
	auto user = FindUserBySessionId( sessionId );
	if( !user )
	{
		Log::Error( "RemoveUser : [{}] not found", sessionId );
		return;
	}

	RemoveUser( user );
}

void UserManager::RemoveUser( const sptr_socket socket )
{
	auto user = FindUserBySocket( socket );
	if( !user )
	{
		Log::Error( "RemoveUser : [{}] not found", socket->remote_ip );
		return;
	}

	RemoveUser( user );
}

void UserManager::Disconnect( sptr_socket socket, const std::string reason )
{
	if( nullptr == socket )
	{
		return;
	}

	Log::Debug( "DisconnectSocket : [{}]. Reason: {}", socket->remote_ip, reason );

	socket->send( NotifyForcedLogout() );
	socket->flag.disconnected_wait = true;

	RemoveUser( socket );
}

void UserManager::Disconnect( sptr_user user, const std::string reason )
{
	if( nullptr == user )
	{
		return;
	}

	if( user->sock != nullptr )
	{
		user->sock->send( NotifyForcedLogout() );
		user->sock->flag.disconnected_wait = true;
	}

	Log::Debug( "DisconnectUser : [{}]. Reason: {}", user->m_sessionId, reason );

	RemoveUser( user );
}

sptr_user UserManager::FindUserBySessionId( const std::wstring &sessionId )
{
	std::lock_guard<std::mutex> lock( m_mutex );
	auto it = std::find_if( m_users.begin(), m_users.end(), [ & ]( const sptr_user &user )
	{
		return user->m_sessionId == sessionId;
	} );
	return ( it != m_users.end() ) ? *it : nullptr;
}

sptr_user UserManager::FindUserBySocket( const sptr_socket &socket )
{
	std::lock_guard<std::mutex> lock( m_mutex );
	auto it = std::find_if( m_users.begin(), m_users.end(), [ & ]( const sptr_user &user )
	{
		return user->sock == socket;
	} );
	return ( it != m_users.end() ) ? *it : nullptr;
}

sptr_user UserManager::RecoverUserBySession( const std::wstring &sessionId, const sptr_socket &socket )
{
	auto user = FindUserBySocket( socket );

	if( user == nullptr )
	{
		Log::Error( "RecoverUserBySession: User not found for socket: {}", socket->remote_ip );
		return nullptr;
	}

	if( sessionId.empty() )
	{
		Log::Error( "RecoverUserBySession: Empty session ID provided." );
		return nullptr;
	}

	const auto [account_id, character_id] = Database::Get().GetSession( sessionId, socket->remote_ip );

	if( account_id < 0 )
	{
		Log::Error( "RecoverUserBySession: Failed to get session for account ID: {}, session ID: {}", account_id, sessionId );
		return nullptr;
	}

	user->m_accountId = account_id;
	user->m_sessionId = sessionId;
	user->m_character = Database::Get().LoadCharacterData( account_id, character_id );

	Log::Debug( "RecoverUserBySession: User recovered with session ID: {}, account ID: {}", sessionId, account_id );

	return user;
}

int32_t UserManager::GetUserCount() const
{
	return static_cast< int32_t >( m_users.size() );
}

std::vector<sptr_user> UserManager::GetUserList()
{
	std::lock_guard<std::mutex> lock( m_mutex );
	return m_users;
}
