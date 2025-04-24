
#include "logging.h"

static const char* LOG_PATH[] = {
	"./generic",
	"./debug",
	"./error"
};

Log::Log()
{
	current_open_hour = 0;
}

Log::~Log()
{
}

const char *Log::GetTimeStamp()
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

void Log::CheckFileStatus( LOG_TYPE type )
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

void Log::WriteToLog( LOG_TYPE type, std::string format )
{
	log_lock.lock();

	HANDLE hConsole = GetStdHandle( STD_OUTPUT_HANDLE );
	SetConsoleTextAttribute( hConsole, type + 10 );

	switch( type )
	{
		case LOG_TYPE::log_generic: printf( "[INFO]: " ); break;
		case LOG_TYPE::log_debug: printf( "[DEBUG]: " ); break;
		case LOG_TYPE::log_warn: printf( "[WARN]: " ); break;
		case LOG_TYPE::log_error: printf( "[ERROR]: " ); break;
	}
	
	SetConsoleTextAttribute( hConsole, 15 );

	printf( "%s\n", format.c_str() );

	CheckFileStatus( type );
	file_stream[ type ] << GetTimeStamp() << format << '\n';
	file_stream[ type ].close();
	log_lock.unlock();
}

void Log::Info( std::string format, ... )
{
	std::vector< char > buf( 512 );
	va_list args;
	va_start( args, format );
	vsnprintf_s( &buf[ 0 ], buf.size(), buf.size() + strlen( format.c_str() ), format.c_str(), args );
	va_end( args );

	WriteToLog( log_generic, &buf[ 0 ] );
}

void Log::Warn( std::string format, ... )
{
	std::vector< char > buf( 512 );
	va_list args;
	va_start( args, format );
	vsnprintf_s( &buf[ 0 ], buf.size(), buf.size() + strlen( format.c_str() ), format.c_str(), args );
	va_end( args );

	WriteToLog( log_warn, &buf[ 0 ] );
}

void Log::Debug( std::string format, ... )
{
	std::vector< char > buf( 512 );
	va_list args;
	va_start( args, format );
	vsnprintf_s( &buf[ 0 ], buf.size(), buf.size() + strlen( format.c_str() ), format.c_str(), args );
	va_end( args );

	WriteToLog( log_debug, &buf[ 0 ] );
}

void Log::Error( std::string format, ... )
{
	std::vector< char > buf( 512 );
	va_list args;
	va_start( args, format );
	vsnprintf_s( &buf[ 0 ], buf.size(), buf.size() + strlen( format.c_str() ), format.c_str(), args );
	va_end( args );

	WriteToLog( log_error, &buf[ 0 ] );
}

void Log::Packet( std::vector<uint8_t> p, size_t size, bool send )
{
	std::lock_guard<std::mutex> lock( log_lock );

	HANDLE hConsole = GetStdHandle( STD_OUTPUT_HANDLE );

	SetConsoleTextAttribute( hConsole, 15 );

	uint16_t i = 0;
	uint8_t line[ 16 ] = {};
	uint8_t r = 0;

	SetConsoleTextAttribute( hConsole, send ? 11 : 10 );
	printf( "(%s)(00|01|02|03|04|05|06|07|08|09|0A|0B|0C|0D|0E|0F)\n", send ? "SEND" : "RECV" );
	SetConsoleTextAttribute( hConsole, 15 );

	while( i < size )
	{
		if( i % 16 == 0 )
		{
			if( i > 0 )
			{
				printf( " " );
				for( uint8_t j = 0; j < r; ++j )
				{
					char c = line[ j ];
					printf( "%c", ( c >= 0x20 && c <= 0x7E ) ? c : '.' );
				}
				printf( "\n" );
			}
			printf( "(%04X) ", i );
			r = 0;
		}

		line[ r++ ] = p[ i ];


		if( i == 4 || i == 5 )
		{
			SetConsoleTextAttribute( hConsole, send ? 11 : 10 );
			printf( "%02X ", p[ i ] );
			SetConsoleTextAttribute( hConsole, 15 );
		}
		else
		{
			printf( "%02X ", p[ i ] );
		}

		++i;
	}


	if( r > 0 )
	{
		for( uint8_t j = r; j < 16; ++j )
			printf( "   " ); // Pad to align character section

		printf( " " );
		for( uint8_t j = 0; j < r; ++j )
		{
			char c = line[ j ];
			printf( "%c", ( c >= 0x20 && c <= 0x7E ) ? c : '.' );
		}
	}

	printf( "\n\n" );
}
