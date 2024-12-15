#include "stdafx.h"
#include "global_define.h"

void ShowStartup()
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

	if( false == ServerConfig::Get().Load( "config.json" ) )
	{
		return 0;
	}

	Initialize_Global();

	logging.information( "Server Start..." );

	while( true )
	{
		server_time->Tick();

		process_networking();

		std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );
	}

    return 0;
}