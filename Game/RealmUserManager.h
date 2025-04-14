#pragma once

#include "RealmUser.h"

class RealmUserManager {
private:
	static const int MAX_SESSION_ID_LENGTH = 16;
	static inline std::unique_ptr< RealmUserManager > m_instance;
	static inline std::mutex m_mutex;

	std::vector< sptr_user > m_users;
public:
	static RealmUserManager& Get()
	{
		std::lock_guard< std::mutex > lock( m_mutex );
		if( m_instance == nullptr )
		{
			m_instance.reset( new RealmUserManager() );
		}

		return *m_instance;
	}

	RealmUserManager( const RealmUserManager & ) = delete;
	RealmUserManager &operator=( const RealmUserManager & ) = delete;
	RealmUserManager();
	~RealmUserManager();

public:
	static std::wstring GenerateSessionId();
	sptr_user CreateUser( sptr_socket socket );
	void RemoveUser( sptr_user user );
	void RemoveUser( const std::wstring &sessionId );
	void RemoveUser( const sptr_socket socket );

	sptr_user GetUser( const std::wstring &sessionId );
	sptr_user GetUser( const sptr_socket socket );

	int32_t GetUserCount() const;
};