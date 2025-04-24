#include "stdafx.h"
#include "global_define.h"

static void ShowStartup()
{
	printf
	(
		"------------------------------------------------------\n"
		"Norrath Server Build Version %s\n"
		"------------------------------------------------------\n\n",
		__DATE__
	);
}

static bool NetworkStartup()
{
	WORD wVersionRequest = MAKEWORD( 2, 2 );
	WSADATA wsaData;

	if( WSAStartup( wVersionRequest, &wsaData ) != 0 )
	{
		Log::Error( "WSAStartup() failed" );
		return false;
	}

	return true;
}

int main()
{
	ShowStartup();

	if( !NetworkStartup() )
	{
		Log::Error( "Could not initialize network." );
		return 0;
	}

	Log::Info( "Server Start..." );

	if( !Config::Load( "config.ini" ) )
	{
		Log::Error( "Failed to load configuration file." );
		return 0;
	}

	auto &lobby_server = LobbyServer::Get();
	lobby_server.Start( Config::service_ip );

	auto &discovery_server = DiscoveryServer::Get();
	discovery_server.Start( Config::service_ip, Config::discovery_port );

	while( true )
	{
		if( !lobby_server.isRunning() )
		{
			Log::Error( "Lobby Server is not running. Exiting." );
			break;
		}

		if( !discovery_server.isRunning() )
		{
			Log::Error( "Discovery Server is not running. Exiting." );
			break;
		}

		std::this_thread::sleep_for( std::chrono::milliseconds( 250 ) );
	}

	lobby_server.Stop();
	discovery_server.Stop();

	return 0;
}