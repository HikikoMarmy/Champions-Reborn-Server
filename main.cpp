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

int main()
{
	ShowStartup();

	WORD wVersionRequest = MAKEWORD( 2, 2 );
	WSADATA wsaData;

	if( WSAStartup( wVersionRequest, &wsaData ) != 0 )
	{
		Log::Error( "WSAStartup() failed" );
		return 0;
	}

	Log::Info( "Server Start..." );

	auto gateway_server = GatewayServer::Get();
	gateway_server->Start( "192.168.1.248", 40801 );

	LobbyServer::Get().Start( "192.168.1.248", 40810 );
	DiscoveryServer::Get().Start( "192.168.1.248", 40820 );

	while( true )
	{
		if( !gateway_server->isRunning() )
		{
			Log::Error( "Gateway Server is not running. Exiting." );
			break;
		}

		//if( !lobby_server.isRunning() )
		//{
		//	Log::Error( "Lobby Server is not running. Exiting." );
		//	break;
		//}
		//
		//if( !discovery_server.isRunning() )
		//{
		//	Log::Error( "Discovery Server is not running. Exiting." );
		//	break;
		//}

		std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );
	}

	gateway_server->Stop();

    return 0;
}