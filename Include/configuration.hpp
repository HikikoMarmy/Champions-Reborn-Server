#pragma once

#include <fstream>
#include <string>

class Config
{
public:
	static bool Load( std::string filename );

	static inline std::string service_ip;
	static inline uint16_t con_lobby_port;
	static inline uint16_t rta_lobby_port;
	static inline uint16_t discovery_port;
};