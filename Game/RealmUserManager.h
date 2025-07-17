#pragma once

#include <memory>
#include <mutex>
#include <vector>
#include <string>
#include <random>

#include "RealmUser.h"

class UserManager {
private:

public:
	static UserManager &Get()
	{
		static UserManager instance;
		return instance;
	}

	UserManager( const UserManager & ) = delete;
	UserManager &operator=( const UserManager & ) = delete;
	UserManager();
	~UserManager();

	std::wstring GenerateSessionId();
	sptr_user CreateUser( sptr_socket socket, RealmGameType clientType );
	void RemoveUser( sptr_user user );
	void RemoveUser( const std::wstring &sessionId );
	void RemoveUser( const sptr_socket socket );
	void Disconnect( sptr_socket socket, const std::string reason );
	void Disconnect( sptr_user user, const std::string reason );

	sptr_user FindUserBySessionId( const std::wstring &sessionId );
	sptr_user FindUserBySocket( const sptr_socket &socket );
	sptr_user FindUserByChatHandle( const std::wstring &handle );
	int32_t GetUserCount() const;
	std::vector< sptr_user > GetUserList();

	void NotifyFriendsOnlineStatus( const sptr_user &user, bool onlineStatus );

private:
	std::mutex m_mutex;
	std::vector< sptr_user > m_users;
	std::mt19937 rng;
};