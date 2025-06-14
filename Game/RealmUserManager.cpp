#include "RealmUserManager.h"
#include "GameSessionManager.h"

#include "../Network/Event/NotifyForcedLogout.h"
#include "../Common/Constant.h"
#include "../Database/Database.h"
#include "../logging.h"

RealmUserManager::RealmUserManager()
{
	std::random_device rd;
	rng.seed( rd() );

	m_users.clear();
}

RealmUserManager::~RealmUserManager()
{
}

std::wstring RealmUserManager::GenerateSessionId()
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

sptr_user RealmUserManager::CreateUser( sptr_socket socket, RealmGameType clientType )
{
	Log::Debug( "ClientManager::CreateUser() - Created new user" );

	auto user = std::make_shared< RealmUser >();

	user->sock = socket;
	user->m_gameType = clientType;

	std::lock_guard< std::mutex > lock( m_mutex );
	m_users.push_back( user );

	return user;
}

void RealmUserManager::RemoveUser( sptr_user user )
{
	auto it = std::find( m_users.begin(), m_users.end(), user );
	if( it == m_users.end() )
	{
		Log::Error( "RemoveUser : [%S] not found", user->m_sessionId.c_str() );
		return;
	}

	Database::Get().DeleteSession( user->m_sessionId );
	GameSessionManager::Get().OnDisconnectUser( user );

	Log::Debug( "RemoveUser : [%S]", user->m_sessionId.c_str() );

	std::lock_guard< std::mutex > lock( m_mutex );
	m_users.erase( it );
}

void RealmUserManager::RemoveUser( const std::wstring &sessionId )
{
	auto user = FindUserBySessionId( sessionId );
	if( !user )
	{
		Log::Error( "RemoveUser : [%S] not found", sessionId.c_str() );
		return;
	}

	RemoveUser( user );
}

void RealmUserManager::RemoveUser( const sptr_socket socket )
{
	auto user = FindUserBySocket( socket );
	if( !user )
	{
		Log::Error( "RemoveUser : [%S] not found", socket->remote_ip.c_str() );
		return;
	}

	RemoveUser( user );
}

void RealmUserManager::DisconnectUser( sptr_user user, const std::string reason )
{
	if( nullptr == user )
	{
		return;
	}

	if( user->sock != nullptr )
	{
		NotifyForcedLogout notifyLogout;
		user->sock->send( notifyLogout );
		user->sock->flag.disconnected_wait = true;
	}

	Log::Debug( "DisconnectUser : [%S]. Reason: %s", user->m_sessionId.c_str(), reason.c_str() );

	RemoveUser( user );
}

sptr_user RealmUserManager::FindUserBySessionId( const std::wstring &sessionId )
{
	std::lock_guard<std::mutex> lock( m_mutex );
	auto it = std::find_if( m_users.begin(), m_users.end(), [ & ]( const sptr_user &user )
	{
		return user->m_sessionId == sessionId;
	} );
	return ( it != m_users.end() ) ? *it : nullptr;
}

sptr_user RealmUserManager::FindUserBySocket( const sptr_socket &socket )
{
	std::lock_guard<std::mutex> lock( m_mutex );
	auto it = std::find_if( m_users.begin(), m_users.end(), [ & ]( const sptr_user &user )
	{
		return user->sock == socket;
	} );
	return ( it != m_users.end() ) ? *it : nullptr;
}

int32_t RealmUserManager::GetUserCount() const
{
	return static_cast< int32_t >( m_users.size() );
}
