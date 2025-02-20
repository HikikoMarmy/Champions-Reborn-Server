
#include "global_define.h"
#include "configuration.h"

bool Config::Load( std::string filename )
{
	service_ip = "0.0.0.0";
	gateway_port = 40801;
	lobby_port = 40802;
	discovery_port = 40101;

	// Read configuration from ini file
	std::ifstream file( filename );
	if( !file.is_open() )
	{
		return false;
	}

	std::string line;

	while( std::getline( file, line ) )
	{
		if( line.empty() || line[0] == '#' || line[0] == ';' )
		{
			continue;
		}

		size_t pos = line.find( '=' );
		if( pos == std::string::npos )
		{
			continue;
		}

		std::string key = line.substr( 0, pos );
		std::string value = line.substr( pos + 1 );

		if( key == "service_ip" )
		{
			service_ip = value;
		}
		else if( key == "gateway_port" )
		{
			gateway_port = std::stoi( value );
		}
		else if( key == "lobby_port" )
		{
			lobby_port = std::stoi( value );
		}
		else if( key == "discovery_port" )
		{
			discovery_port = std::stoi( value );
		}
	}

	return true;
}