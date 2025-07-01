#include "stdafx.h"

#include <atomic>
#include <csignal>
#include <winsock2.h>

#include "logging.h"
#include "configuration.h"
#include "Database/Database.h"
#include "Lobby Server/LobbyServer.h"
#include "Discovery Server/DiscoveryServer.h"

std::atomic< bool > g_isRunning( true );

static void SignalHandler( int signal )
{
	if( signal == SIGINT || signal == SIGTERM )
	{
		g_isRunning = false;
	}
}

static void ShowStartup()
{
	printf
	(
		"------------------------------------------------------\n"
		"Champions Reborn | Server Build Version %s\n"
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

	std::signal( SIGINT, SignalHandler );
	std::signal( SIGTERM, SignalHandler );

	Log::Info( "Server Start..." );

	if( !NetworkStartup() )
	{
		Log::Error( "Could not initialize network." );
		return 0;
	}

	if( !Config::Load( "config.ini" ) )
	{
		Log::Error( "Failed to load configuration file." );
		return 0;
	}

	auto &lobby_server = LobbyServer::Get();
	lobby_server.Start( Config::service_ip );

	auto &discovery_server = DiscoveryServer::Get();
	discovery_server.Start( Config::service_ip, Config::discovery_port );

	auto &database = Database::Get();

	while( g_isRunning )
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

		database.Process();

		std::this_thread::sleep_for( std::chrono::milliseconds( 250 ) );
	}

	Log::Info( "Shutting down servers..." );

	lobby_server.Stop();
	discovery_server.Stop();

	return 0;
}