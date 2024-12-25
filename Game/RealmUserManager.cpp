#include "../global_define.h"

RealmUserManager::RealmUserManager()
{
	m_users.clear();
}

RealmUserManager::~RealmUserManager()
{
}

std::wstring RealmUserManager::GenerateSessionId()
{
	// TODO : Use something better than rand()
	std::wstring sessionId;
	for( int i = 0; i < MAX_SESSION_ID_LENGTH; i++ )
	{
		sessionId += L"0123456789ABCDEF"[ rand() % 16 ];
	}

	return sessionId;
}

sptr_user RealmUserManager::CreateUser( sptr_tcp_socket socket, std::wstring userId, std::wstring userPw )
{
	Log::Debug( "ClientManager::CreateUser() - Created new user" );

	auto user = std::make_shared< RealmUser >();

	user->m_sessionId = GenerateSessionId();
	user->m_realmSocket = socket;

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
	
	Log::Debug( "RemoveUser : [%S]", user->m_sessionId.c_str() );
	m_users.erase( it );
}

void RealmUserManager::RemoveUser( const std::wstring &sessionId )
{
	auto it = std::find_if( m_users.begin(), m_users.end(), [ &sessionId ]( sptr_user user )
	{
		return user->m_sessionId == sessionId;
	} );

	if( it == m_users.end() )
	{
		Log::Error( "RemoveUser : [%S] not found", sessionId.c_str() );
		return;
	}

	Log::Debug( "RemoveUser : [%S]", sessionId.c_str() );
	m_users.erase( it );
}

sptr_user RealmUserManager::GetUser( const std::wstring &sessionId )
{
	for( auto &user : m_users )
	{
		if( user->m_sessionId == sessionId )
		{
			return user;
		}
	}

	return nullptr;
}

sptr_user RealmUserManager::GetUser( sptr_tcp_socket socket )
{
	for( auto &user : m_users )
	{
		if( user->m_realmSocket == socket )
		{
			return user;
		}
	}

	return nullptr;
}

sptr_user RealmUserManager::GetUser( sptr_udp_socket socket )
{
	for( auto &user : m_users )
	{
		if( user->m_discoverySocket == socket )
		{
			return user;
		}
	}

	return nullptr;
}
