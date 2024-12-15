#pragma once

#include <fstream>
#include <string>

class ServerConfig
{
public:
	static ServerConfig &Get()
	{
		static ServerConfig instance;
		return instance;
	}

	ServerConfig();
	~ServerConfig();

	bool Load( std::string filename );

	std::string service_ip;
	uint16_t gateway_port;
	uint16_t session_port;
	uint16_t broker_port;
};