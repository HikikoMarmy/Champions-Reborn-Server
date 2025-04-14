#include "..\global_define.h"

int32_t Utility::round_up( int32_t numToRound, int32_t multiple )
{
	if( multiple == 0 )
		return numToRound;

	int32_t remainder = abs( numToRound ) % multiple;
	if( remainder == 0 )
		return numToRound;

	if( numToRound < 0 )
		return -( abs( numToRound ) - remainder );
	else
		return numToRound + multiple - remainder;
}

int32_t Utility::round_down( int32_t numToRound, int32_t multiple )
{
	if( multiple == 0 )
		return numToRound;

	int32_t remainder = abs( numToRound ) % multiple;
	if( remainder == 0 )
		return numToRound;

	if( numToRound < 0 )
		return -( abs( numToRound ) + remainder );
	else
		return numToRound - remainder;
}

uint16_t Utility::swap_endian( uint16_t val )
{
	return ( val << 8 ) | ( val >> 8 );
}

uint32_t Utility::swap_endian( uint32_t val )
{
	return ( ( val << 24 ) & 0xFF000000 ) |
		( ( val << 8 ) & 0x00FF0000 ) |
		( ( val >> 8 ) & 0x0000FF00 ) |
		( ( val >> 24 ) & 0x000000FF );
}
