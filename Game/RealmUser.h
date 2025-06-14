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

public:
	sptr_socket		sock;			// For Realm Lobby

	RealmGameType	m_gameType;		// Champions of Norrath or Return to Arms
	std::wstring	m_sessionId;	// Temporary Session ID
	int64_t			m_accountId;	// Unique ID of the account

	int32_t					m_characterId;	// ID of selected Net Character
	sptr_realm_character	m_character;	// Currently selected character

	bool	m_isHost;	// True if this user is the host of a realm
	int32_t	m_gameId;	// Unique ID of the realm

	std::string m_localAddr;		// Local IP address of the user, passed from the client during Realm creation.
	std::string m_discoveryAddr;	
	int32_t m_discoveryPort;		
};

using sptr_user = std::shared_ptr< RealmUser >;
