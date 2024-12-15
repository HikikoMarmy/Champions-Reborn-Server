#pragma once

#include <fstream>

class CLogManager {
private:
	enum LOG_TYPE {
		log_generic = 0,
		log_debug,
		log_error,
		num_log_type
	};

	uint8_t current_open_hour;
	std::fstream file_stream[ num_log_type ];

	const char* get_time_stamp();
	void check_file_status( LOG_TYPE type );
	void write_log( LOG_TYPE type, std::string format );
public:
	CLogManager()
	{

	}

	~CLogManager()
	{
		for( uint8_t i = 0; i < num_log_type; i++ )
		{
			if( file_stream[ i ].is_open() )
			{
				file_stream[ i ].close();
			}
		}
	}

	void information( std::string format, ... );
	void debug( std::string format, ... );
	void error( std::string format, ... );
	void packet( std::vector< uint8_t > p, bool send );
	void packet( std::vector< uint8_t > p, uint32_t size, bool send );
};