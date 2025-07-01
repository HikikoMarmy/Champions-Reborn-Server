#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <mutex>
#include <sstream>
#include <type_traits>
#include <windows.h>

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

	static const char *GetTimeStamp();
	static void CheckFileStatus( LOG_TYPE type );
	static void WriteToLog( LOG_TYPE type, const std::string &message );

	static std::string WideToMulti( const std::wstring &input )
	{
		if( input.empty() ) return "";
		int sizeNeeded = WideCharToMultiByte( CP_UTF8, 0, input.data(), ( int )input.size(), nullptr, 0, nullptr, nullptr );
		std::string result( sizeNeeded, 0 );
		WideCharToMultiByte( CP_UTF8, 0, input.data(), ( int )input.size(), std::data( result ), sizeNeeded, nullptr, nullptr );
		return result;
	}

	template<typename T>
	static std::string ToString( const T &value )
	{
		if constexpr( std::is_same_v<T, std::wstring> )
		{
			return WideToMulti( value );
		}
		else if constexpr( std::is_same_v<T, std::string> )
		{
			return value;
		}
		else
		{
			std::ostringstream oss;
			oss << value;
			return oss.str();
		}
	}

	template<typename... Args>
	static std::string Format( const std::string &formatStr, Args&&... args )
	{
		std::ostringstream oss;
		size_t last = 0, next = 0;
		std::vector<std::string> values = { ToString( std::forward<Args>( args ) )... };
		size_t index = 0;
		while( ( next = formatStr.find( "{}", last ) ) != std::string::npos )
		{
			oss << formatStr.substr( last, next - last );
			if( index < values.size() )
			{
				oss << values[ index++ ];
			}
			else
			{
				oss << "{}"; // unmatched placeholder
			}
			last = next + 2;
		}
		oss << formatStr.substr( last );
		return oss.str();
	}

public:
	static Log &Get()
	{
		static Log instance;
		return instance;
	}

	Log();
	~Log();

	template<typename... Args>
	static void Info( const std::string &format, Args&&... args )
	{
		WriteToLog( log_generic, Format( format, std::forward<Args>( args )... ) );
	}

	template<typename... Args>
	static void Warn( const std::string &format, Args&&... args )
	{
		WriteToLog( log_warn, Format( format, std::forward<Args>( args )... ) );
	}

	template<typename... Args>
	static void Debug( const std::string &format, Args&&... args )
	{
		WriteToLog( log_debug, Format( format, std::forward<Args>( args )... ) );
	}

	template<typename... Args>
	static void Error( const std::string &format, Args&&... args )
	{
		WriteToLog( log_error, Format( format, std::forward<Args>( args )... ) );
	}

	static void Packet( std::vector<uint8_t> p, size_t size, bool send );
	static void ToFile( std::string prefix, std::vector<uint8_t> p, size_t size );
};
