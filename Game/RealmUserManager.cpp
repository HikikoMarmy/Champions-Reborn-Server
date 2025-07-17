#include "RealmUserManager.h"
#include "GameSessionManager.h"
#include "ChatRoomManager.h"

#include "../Network/Event/NotifyForcedLogout.h"
#include "../Network/Event/NotifyFriendStatus.h"
#include "../Database/Database.h"
#include "../Common/Constant.h"
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

	GameSessionManager::Get().OnDisconnectUser( user );
	ChatRoomManager::Get().OnDisconnectUser( user );

	NotifyFriendsOnlineStatus( user, false );

	Log::Debug( "RemoveUser : [{}][{}]", user->m_username, user->m_sessionId );

	std::lock_guard< std::mutex > lock( m_mutex );
	m_users.erase( it );
}

void UserManager::RemoveUser( const std::wstring &sessionId )
{
	if( auto user = FindUserBySessionId( sessionId ) )
	{
		RemoveUser( user );
	}
}

void UserManager::RemoveUser( const sptr_socket socket )
{
	if( auto user = FindUserBySocket( socket ) )
	{
		RemoveUser( user );
	}
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

sptr_user UserManager::FindUserByChatHandle( const std::wstring &handle )
{
	std::lock_guard<std::mutex> lock( m_mutex );
	auto it = std::find_if( m_users.begin(), m_users.end(), [ & ]( const sptr_user &user )
	{
		return user->m_chatHandle == handle;
	} );
	return ( it != m_users.end() ) ? *it : nullptr;
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

void UserManager::NotifyFriendsOnlineStatus( const sptr_user &user, bool onlineStatus )
{
	if( !user || user->m_friendList.empty() )
	{
		return;
	}

	const auto notifyFriend = NotifyFriendStatus( user->m_chatHandle, onlineStatus );
	for( const auto &friendHandle : user->m_friendList )
	{
		auto friendUser = FindUserByChatHandle( friendHandle );
		if( friendUser && friendUser->sock )
		{
			friendUser->sock->send( notifyFriend );
		}
	}
}
