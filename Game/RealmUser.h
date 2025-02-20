#pragma once

#include "RealmCharacterData.h"

enum class DiscoveryState
{
	None,
	Host_Waiting,
	Initial_Connect,
	Negotiating,
	Joining,
	Joined
};

class RealmUser {
public:
	RealmUser();
	~RealmUser();

public:
	sptr_tcp_socket tcp;			// For Realm Lobby
	sptr_udp_socket udp;			// For Discovery

	std::wstring	session_id;		// Unique Session ID for the user (Generated at login)

	bool	is_host;
	bool	is_ready;
	int32_t	game_id;

	std::string local_addr;	
	std::string discovery_addr;
	int32_t discovery_port;
	DiscoveryState discovery_state;

	std::string player_name;
	std::string player_class;
	int32_t player_level;
};

typedef std::shared_ptr< RealmUser >	sptr_user;
