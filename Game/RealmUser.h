#pragma once

#include <string>
#include <memory>
#include <array>

#include "RealmCharacter.h"

#include "../Common/Constant.h"
#include "../Network/RealmSocket.h"

class RealmUser {
public:
	RealmUser();
	~RealmUser();

	bool operator==( const RealmUser &other ) const
	{
		return m_sessionId == other.m_sessionId || m_accountId == other.m_accountId;
	}

	bool operator<( const RealmUser &other ) const
	{
		if( m_sessionId != other.m_sessionId )
			return m_sessionId < other.m_sessionId;
		return m_accountId < other.m_accountId;
	}

public:
	sptr_socket		sock;			// For Realm Lobby

	RealmGameType	m_gameType;		// Champions of Norrath or Return to Arms
	int64_t			m_accountId;	// Unique ID of the account
	std::wstring	m_sessionId;	// Temporary Session ID
	std::wstring	m_username;		// Username of the user
	std::wstring	m_chatHandle;	

	bool			m_isLoggedIn;	// True if the user has successfully authenticated and logged in
	bool			m_isHost;		// True if this user is the host of a realm
	int32_t			m_gameId;		// Unique ID of the realm

	int32_t			m_publicRoomId;		// Used for public chat rooms
	int32_t			m_privateRoomId;	// Used for private chat rooms

	std::string		m_localAddr;		// Local IP address of the user, passed from the client during Realm creation.
	int32_t			m_localPort;
	std::string		m_discoveryAddr;
	int32_t			m_discoveryPort;

	int32_t 		m_characterId;
	sptr_realm_character m_character;
};

using sptr_user = std::shared_ptr< RealmUser >;
using wptr_user = std::weak_ptr< RealmUser >;
