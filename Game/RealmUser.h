#pragma once

class RealmUser {
public:
	RealmUser();
	~RealmUser();

public:
	sptr_socket sock;				// For Realm Lobby

	RealmClientType m_clientType;

	std::wstring	m_sessionId;	// Unique Session ID for the user (Generated at login)

	bool	m_isHost;				// True if this user is the host of a realm
	int32_t	m_gameId;				// Unique ID of the realm

	std::string m_localAddr;		// Local IP address of the user, passed from the client during realm creation.
	std::string m_discoveryAddr;
	int32_t m_discoveryPort;
};

typedef std::shared_ptr< RealmUser >	sptr_user;
