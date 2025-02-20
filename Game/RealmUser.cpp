#include "../global_define.h"

RealmUser::RealmUser()
{
	tcp = nullptr;
	udp = std::make_shared<RealmUDPSocket>();

	session_id = L"";

	local_addr = "";
	discovery_addr = "";
	discovery_port = 0;
	discovery_state = DiscoveryState::None;

	is_host = false;
	is_ready = false;

	game_id = 0;

	player_name = "";
	player_class = "";
	player_level = 0;
}

RealmUser::~RealmUser()
{
	if( tcp )
	{
		tcp->flag.disconnected = true;
		tcp.reset();
	}

	if( udp )
	{
		udp->flag.disconnected = true;
		udp.reset();
	}

	session_id = L"";

	local_addr = "";
	discovery_addr = "";
	discovery_port = 0;
	discovery_state = DiscoveryState::None;

	is_host = false;
	is_ready = false;

	game_id = 0;

	player_name = "";
	player_class = "";
	player_level = 0;

}