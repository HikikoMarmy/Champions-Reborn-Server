#pragma once

class RealmPlayerData
{
public:
	char local_addr[24];		// [0x0]
	char discovery_addr[24];	// [0x18]
	int32_t discovery_port;		// [0x30]

	char player_name[24];
	char player_class[24];
	int32_t player_level;
};