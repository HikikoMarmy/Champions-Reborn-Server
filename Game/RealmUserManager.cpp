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
	// TODO : Maybe use something better than rand()
	// but it is just a temporary ID
	std::wstring sessionId;
	for( int i = 0; i < MAX_SESSION_ID_LENGTH; i++ )
	{
		sessionId += L"0123456789ABCDEF"[ rand() % 16 ];
	}

	return sessionId;
}

sptr_user RealmUserManager::CreateUser( sptr_socket socket )
{
	Log::Debug( "ClientManager::CreateUser() - Created new user" );

	auto user = std::make_shared< RealmUser >();

	user->m_sessionId = GenerateSessionId();
	user->sock = socket;

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
	
	GameSessionManager::Get().OnDisconnectUser( user );
	
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

	RemoveUser((*it));
}

void RealmUserManager::RemoveUser( const sptr_socket socket )
{
	auto it = std::find_if( m_users.begin(), m_users.end(), [ &socket ]( sptr_user user )
	{
		return user->sock == socket;
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
		if( user->m_sessionId == sessionId )
		{
			return user;
		}
	}

	return nullptr;
}

sptr_user RealmUserManager::GetUser( const sptr_socket socket )
{
	auto it = std::find_if(m_users.begin(), m_users.end(), [&socket](sptr_user user)
	{
		return user->sock == socket;
	});

	if (it == m_users.end())
	{
		Log::Error("GetUser : [%S] not found", socket->remote_ip.c_str());
		return nullptr;
	}

	return (*it);
}

int32_t RealmUserManager::GetUserCount() const
{
	return static_cast< int32_t >( m_users.size() );
}
