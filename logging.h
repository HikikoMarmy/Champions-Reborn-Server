#pragma once

#include <fstream>
#include <mutex>
#include <cstdarg>
#include <iostream>
#include <string>
#include <vector>
#include <Windows.h>

class Log {
private:
	enum LOG_TYPE {
		log_generic = 0,
		log_debug,
		log_error,
		log_warn,
		num_log_type
	};

	static inline int32_t current_open_hour;
	static inline std::fstream file_stream[ num_log_type ];
	static inline std::mutex log_lock;

	static const char* GetTimeStamp();
	static void CheckFileStatus( LOG_TYPE type );
	static void WriteToLog( LOG_TYPE type, std::string format );
public:

	static Log &Get()
	{
		static Log instance;
		return instance;
	}

	Log();
	~Log();

	static void Info( std::string format, ... );
	static void Warn( std::string format, ... );
	static void Debug( std::string format, ... );
	static void Error( std::string format, ... );
	static void Packet( std::vector< uint8_t > p, bool send );
	static void Packet( std::vector< uint8_t > p, uint32_t size, bool send );

	
};