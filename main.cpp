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
	WORD wVersionRequest = MAKEWORD(2, 2);
	WSADATA wsaData;

	if (WSAStartup(wVersionRequest, &wsaData) != 0)
	{
		Log::Error("WSAStartup() failed");
		return false;
	}

	return true;
}

int main()
{
	ShowStartup();

	if (false == NetworkStartup())
	{
		Log::Error("Could not initialize network.");
		return 0;
	}
	
	Log::Info( "Server Start..." );

	if( !Config::Load( "config.ini" ) )
	{
		Log::Error( "Failed to load configuration file." );
		return 0;
	}

	auto &gateway_server = GatewayServer::Get();
	gateway_server.Start( Config::service_ip, Config::gateway_port );

	auto &lobby_server = LobbyServer::Get();
	lobby_server.Start(Config::service_ip, Config::lobby_port);

	auto &discovery_server = DiscoveryServer::Get();
	discovery_server.Start(Config::service_ip, Config::discovery_port);

	while( true )
	{
		if( !gateway_server.isRunning() )
		{
			Log::Error( "Gateway Server is not running. Exiting." );
			break;
		}

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

		GameSessionManager::Get().Process();

		std::this_thread::sleep_for( std::chrono::milliseconds( 250 ) );
	}

	gateway_server.Stop();
	lobby_server.Stop();
	discovery_server.Stop();

    return 0;
}