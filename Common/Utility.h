#pragma once

#include <cstdint>
#include <string>

namespace Util
{
	int32_t round_up( int32_t numToRound, int32_t multiple );
	int32_t round_down( int32_t numToRound, int32_t multiple );

	uint16_t ByteSwap( uint16_t val );
	uint32_t ByteSwap( uint32_t val );

	template <typename T>
	bool IsInRange( T value, T min, T max )
	{
		return ( value >= min && value <= max );
	}

	std::string WideToUTF8( const std::wstring &wstr );
	std::wstring UTF8ToWide( const std::string &str );
}