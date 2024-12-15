#include "../global_define.h"

static const char* LOG_PATH[] = {
	"./generic",
	"./debug",
	"./error"
};

#include <mutex>
static std::mutex log_lock;

void command_callback( void* ptr, size_t len )
{
	const wchar_t* p = reinterpret_cast< const wchar_t* >( ptr );
	std::wstring command( p, p + len );

	// Temp
	logging.information( "Server command: %S", command.c_str() );
}

const char* CLogManager::get_time_stamp()
{
	static char timestamp[ 64 ] = "";

	time_t t; time( &t );
	struct tm date_tm;
	localtime_s( &date_tm, &t );

	_snprintf_s( timestamp, _TRUNCATE, 63,
				 "[%4d-%02d-%02d %02d:%02d:%02d]: ",
				 ( date_tm.tm_year + 1900 ),
				 ( date_tm.tm_mon + 1 ),
				 date_tm.tm_mday,
				 date_tm.tm_hour,
				 date_tm.tm_min,
				 date_tm.tm_sec );

	return timestamp;
}

void CLogManager::check_file_status( LOG_TYPE type )
{
	time_t t; time( &t );
	struct tm date_tm;
	localtime_s( &date_tm, &t );

	if( current_open_hour != date_tm.tm_hour )
	{
		current_open_hour = date_tm.tm_hour;
		if( file_stream[ type ].is_open() )
		{
			file_stream[ type ].close();
		}
	}

	if( !file_stream[ type ].is_open() )
	{
		char szFileName[ 256 ] = "";

		_snprintf_s( szFileName, _TRUNCATE, 255, "./log/%s/log_%04d.%02d.%02d_%02d.log",
					 LOG_PATH[ type ],
					 ( date_tm.tm_year + 1900 ),
					 ( date_tm.tm_mon + 1 ),
					 date_tm.tm_mday,
					 date_tm.tm_hour );

		file_stream[ type ].open( szFileName, std::fstream::out | std::fstream::app );
	}
}

void CLogManager::write_log( LOG_TYPE type, std::string format )
{
	log_lock.lock();

	HANDLE hConsole = GetStdHandle( STD_OUTPUT_HANDLE );
	SetConsoleTextAttribute( hConsole, type + 10 );

	switch( type )
	{
		case LOG_TYPE::log_generic: printf( "[INFO]: " ); break;
		case LOG_TYPE::log_debug: printf( "[DEBUG]: " ); break;
		case LOG_TYPE::log_error: printf( "[ERROR]: " ); break;
	}
	
	SetConsoleTextAttribute( hConsole, 15 );

	printf( "%s\n", format.c_str() );

	check_file_status( type );
	file_stream[ type ] << get_time_stamp() << format << '\n';
	file_stream[ type ].close();
	log_lock.unlock();
}

void CLogManager::information( std::string format, ... )
{
	std::vector< char > buf( 512 );
	va_list args;
	va_start( args, format );
	vsnprintf_s( &buf[ 0 ], buf.size(), buf.size() + strlen( format.c_str() ), format.c_str(), args );
	va_end( args );

	write_log( log_generic, &buf[ 0 ] );
}

void CLogManager::debug( std::string format, ... )
{
	std::vector< char > buf( 512 );
	va_list args;
	va_start( args, format );
	vsnprintf_s( &buf[ 0 ], buf.size(), buf.size() + strlen( format.c_str() ), format.c_str(), args );
	va_end( args );

	write_log( log_debug, &buf[ 0 ] );
}

void CLogManager::error( std::string format, ... )
{
	std::vector< char > buf( 512 );
	va_list args;
	va_start( args, format );
	vsnprintf_s( &buf[ 0 ], buf.size(), buf.size() + strlen( format.c_str() ), format.c_str(), args );
	va_end( args );

	write_log( log_error, &buf[ 0 ] );
}

void CLogManager::packet( std::vector< uint8_t > p, bool send )
{
	log_lock.lock();

	HANDLE hConsole = GetStdHandle( STD_OUTPUT_HANDLE );

	uint32_t size = _byteswap_ulong( *( uint32_t * )&p[ 0 ] );

	SetConsoleTextAttribute( hConsole, 15 );

	uint16_t i = 0;
	uint16_t r = 0;

	if( send )
	{
		SetConsoleTextAttribute( hConsole, 11 );
		printf( "(SEND)(00|01|02|03|04|05|06|07|08|09|0A|0B|0C|0D|0E|0F)\n" );
		SetConsoleTextAttribute( hConsole, 15 );
	}
	else
	{
		SetConsoleTextAttribute( hConsole, 10 );
		printf( "(RECV)(00|01|02|03|04|05|06|07|08|09|0A|0B|0C|0D|0E|0F)\n" );
		SetConsoleTextAttribute( hConsole, 15 );
	}

	while( i < size )
	{
		if( r == 16 )
		{
			uint8_t *c = &p[ ( i - 16 ) ];
			for( r = 0; r < 16; r++, c++ )
			{
				if( ( ( *c ) >= 0x20 && ( *c ) <= 127 ) && ( isprint( ( *c ) ) ) )
					printf( "%c", ( *c ) );
				else
					printf( "%c", 0x2E );
			}
			r = 0;
		}

		if( i % 16 == 0 )
		{
			SetConsoleTextAttribute( hConsole, ( send ) ? 11 : 10 );
			if( i > 0 ) printf( "\n" );
			printf( "(%04X) ", i );
			SetConsoleTextAttribute( hConsole, 15 );
		}

		if( i == 4 || i == 5 )
		{
			SetConsoleTextAttribute( hConsole, ( send ) ? 11 : 10 );
			printf( "%02X ", p[ i ] );
			SetConsoleTextAttribute( hConsole, 15 );
		}
		else
			printf( "%02X ", p[ i ] );

		i++;
		r++;
	}
	printf( "\n\n" );

	log_lock.unlock();
}

void CLogManager::packet( std::vector<uint8_t> p, uint32_t size, bool send )
{
	log_lock.lock();

	HANDLE hConsole = GetStdHandle( STD_OUTPUT_HANDLE );

	SetConsoleTextAttribute( hConsole, 15 );

	uint16_t i = 0;
	uint16_t r = 0;

	if( send )
	{
		SetConsoleTextAttribute( hConsole, 11 );
		printf( "(SEND)(00|01|02|03|04|05|06|07|08|09|0A|0B|0C|0D|0E|0F)\n" );
		SetConsoleTextAttribute( hConsole, 15 );
	}
	else
	{
		SetConsoleTextAttribute( hConsole, 10 );
		printf( "(RECV)(00|01|02|03|04|05|06|07|08|09|0A|0B|0C|0D|0E|0F)\n" );
		SetConsoleTextAttribute( hConsole, 15 );
	}

	while( i < size )
	{
		if( r == 16 )
		{
			uint8_t *c = &p[ ( i - 16 ) ];
			for( r = 0; r < 16; r++, c++ )
			{
				if( ( ( *c ) >= 0x20 && ( *c ) <= 127 ) && ( isprint( ( *c ) ) ) )
					printf( "%c", ( *c ) );
				else
					printf( "%c", 0x2E );
			}
			r = 0;
		}

		if( i % 16 == 0 )
		{
			SetConsoleTextAttribute( hConsole, ( send ) ? 11 : 10 );
			if( i > 0 ) printf( "\n" );
			printf( "(%04X) ", i );
			SetConsoleTextAttribute( hConsole, 15 );
		}

		if( i == 4 || i == 5 )
		{
			SetConsoleTextAttribute( hConsole, ( send ) ? 11 : 10 );
			printf( "%02X ", p[ i ] );
			SetConsoleTextAttribute( hConsole, 15 );
		}
		else
			printf( "%02X ", p[ i ] );

		i++;
		r++;
	}
	printf( "\n\n" );

	log_lock.unlock();

}
