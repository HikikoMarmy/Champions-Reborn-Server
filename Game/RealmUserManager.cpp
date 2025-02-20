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

sptr_user RealmUserManager::CreateUser( sptr_tcp_socket socket )
{
	Log::Debug( "ClientManager::CreateUser() - Created new user" );

	auto user = std::make_shared< RealmUser >();

	user->session_id = GenerateSessionId();
	user->tcp = socket;

	m_users.push_back( user );

	return user;
}

void RealmUserManager::RemoveUser( sptr_user user )
{
	auto it = std::find( m_users.begin(), m_users.end(), user );
	if( it == m_users.end() )
	{
		Log::Error( "RemoveUser : [%S] not found", user->session_id.c_str() );
		return;
	}

	GameSessionManager::Get().RemoveUser((*it));
	
	Log::Debug( "RemoveUser : [%S]", user->session_id.c_str() );
	m_users.erase( it );
}

void RealmUserManager::RemoveUser( const std::wstring &sessionId )
{
	auto it = std::find_if( m_users.begin(), m_users.end(), [ &sessionId ]( sptr_user user )
	{
		return user->session_id == sessionId;
	} );

	if( it == m_users.end() )
	{
		Log::Error( "RemoveUser : [%S] not found", sessionId.c_str() );
		return;
	}

	RemoveUser((*it));
}

void RealmUserManager::RemoveUser( const sptr_tcp_socket socket )
{
	auto it = std::find_if( m_users.begin(), m_users.end(), [ &socket ]( sptr_user user )
	{
		return user->tcp == socket;
	} );

	if( it == m_users.end() )
	{
		Log::Error( "RemoveUser : [%S] not found", socket->remote_ip.c_str() );
		return;
	}

	RemoveUser((*it));
}

sptr_user RealmUserManager::GetUser( const std::wstring &sessionId )
{
	for( auto &user : m_users )
	{
		if( user->session_id == sessionId )
		{
			return user;
		}
	}

	return nullptr;
}

sptr_user RealmUserManager::GetUser( const sptr_tcp_socket socket )
{
	auto it = std::find_if(m_users.begin(), m_users.end(), [&socket](sptr_user user)
	{
		return user->tcp == socket;
	});

	if (it == m_users.end())
	{
		Log::Error("GetUser : [%S] not found", socket->remote_ip.c_str());
		return nullptr;
	}

	return (*it);
}

sptr_user RealmUserManager::GetUserByAddress( const sockaddr_in* remoteAddr)
{
	for( auto &user : m_users )
	{
		if( nullptr == user->udp )
		{
			continue;
		}

		// Compare the address
		if( user->udp->remote_addr.sin_addr.s_addr == remoteAddr->sin_addr.s_addr &&
			user->udp->remote_addr.sin_port == remoteAddr->sin_port )
		{
			return user;
		}
	}

	return nullptr;
}
