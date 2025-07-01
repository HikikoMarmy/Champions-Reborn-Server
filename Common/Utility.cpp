#include <algorithm>
#include <cmath>
#include <string>
#include <cstdint>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

#include "Utility.h"

int32_t Util::round_up( int32_t numToRound, int32_t multiple )
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

int32_t Util::round_down( int32_t numToRound, int32_t multiple )
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

uint16_t Util::ByteSwap( uint16_t val )
{
	return ( val << 8 ) | ( val >> 8 );
}

uint32_t Util::ByteSwap( uint32_t val )
{
	return ( ( val << 24 ) & 0xFF000000 ) |
		( ( val << 8 ) & 0x00FF0000 ) |
		( ( val >> 8 ) & 0x0000FF00 ) |
		( ( val >> 24 ) & 0x000000FF );
}

std::string Util::IPFromAddr( const sockaddr_in &addr )
{
	char buffer[ INET_ADDRSTRLEN ] = {};
	const char *result = inet_ntop( AF_INET, &addr.sin_addr, buffer, sizeof( buffer ) );

	if( result )
		return std::string( buffer );
	else
		return {};
}

std::string Util::WideToUTF8( const std::wstring &wstr )
{
	if( wstr.empty() ) return {};

	int size_needed = WideCharToMultiByte(
		CP_UTF8, 0, wstr.c_str(), static_cast< int >( wstr.size() ),
		nullptr, 0, nullptr, nullptr
	);

	std::string result( size_needed, 0 );

	WideCharToMultiByte(
		CP_UTF8, 0, wstr.c_str(), static_cast< int >( wstr.size() ),
		&result[ 0 ], size_needed, nullptr, nullptr
	);

	return result;
}

std::wstring Util::UTF8ToWide( const std::string &str )
{
	if( str.empty() ) return {};
	int size_needed = MultiByteToWideChar(
		CP_UTF8, 0, str.c_str(), static_cast< int >( str.size() ),
		nullptr, 0
	);
	std::wstring result( size_needed, 0 );
	MultiByteToWideChar(
		CP_UTF8, 0, str.c_str(), static_cast< int >( str.size() ),
		&result[ 0 ], size_needed
	);
	return result;
}
