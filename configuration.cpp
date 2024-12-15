
#include "global_define.h"
#include "configuration.h"

ServerConfig::ServerConfig()
{
	service_ip.clear();
	gateway_port = 0;
	session_port = 0;
	broker_port = 0;
}

ServerConfig::~ServerConfig()
{
}

bool ServerConfig::Load( std::string filename )
{
	service_ip = "192.168.1.248";
	gateway_port = 40801;
	session_port = 40802;
	broker_port = 3000;	// The game uses 3000 as the default broker port, but also for local net play.

	return true;
}