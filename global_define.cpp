#include "global_define.h"

CLogManager							logging;

std::unique_ptr< CTimer >			server_time;
std::unique_ptr< CClientManager >	client_manager;
std::unique_ptr< CSocketManager >	socket_manager;

void Initialize_Global()
{
	try
	{
		// Start Timer
		server_time = std::make_unique< CTimer >();
		server_time->Start();

		// Start Client Manager
		client_manager = std::make_unique < CClientManager >();

		// Start Network
		socket_manager = std::make_unique < CSocketManager >();
		socket_manager->initialize();
	}
	catch( std::exception e )
	{
		logging.error( "%s\n", e.what() );
	}
}
