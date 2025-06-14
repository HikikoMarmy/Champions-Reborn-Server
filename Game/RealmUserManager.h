#pragma once

#include <memory>
#include <mutex>
#include <vector>
#include <string>
#include <random>

#include "RealmUser.h"

class RealmUserManager {
private:

public:
	static RealmUserManager &Get()
	{
		static RealmUserManager instance;
		return instance;
	}

	RealmUserManager( const RealmUserManager & ) = delete;
	RealmUserManager &operator=( const RealmUserManager & ) = delete;
	RealmUserManager();
	~RealmUserManager();

	std::wstring GenerateSessionId();
	sptr_user CreateUser( sptr_socket socket, RealmGameType clientType );
	void RemoveUser( sptr_user user );
	void RemoveUser( const std::wstring &sessionId );
	void RemoveUser( const sptr_socket socket );
	void DisconnectUser( sptr_user user, const std::string reason );

	sptr_user FindUserBySessionId( const std::wstring &sessionId );
	sptr_user FindUserBySocket( const sptr_socket &socket );
	int32_t GetUserCount() const;

private:
	std::mutex m_mutex;
	std::vector< sptr_user > m_users;
	std::mt19937 rng;
};