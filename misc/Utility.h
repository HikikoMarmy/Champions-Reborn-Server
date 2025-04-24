#pragma once

namespace Utility
{
	int32_t round_up( int32_t numToRound, int32_t multiple );
	int32_t round_down( int32_t numToRound, int32_t multiple );

	uint16_t swap_endian( uint16_t val );
	uint32_t swap_endian( uint32_t val );

	std::string WideToUTF8( const std::wstring &wstr );
}