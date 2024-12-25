#pragma once

/*
#include <memory>
#include "socket.h"

#pragma pack( push, 2 )
struct PacketHeader
{
	uint32_t length;	// The length of the buffer + header
	uint16_t command;	// The command protocol ID
	uint32_t event_id;	// The event ID that we must respond to (this is assigned by the client).
	uint32_t state;		// Connection State (Must be 0, else force reconnect to gateway server)
};
#pragma pack(pop)

constexpr int32_t PACKET_HEADER_SIZE = 14;
static_assert( sizeof( PacketHeader ) == PACKET_HEADER_SIZE, "PacketHeader size mismatch" );

class PacketBuffer
{
public:
	PacketBuffer( uint16_t command, uint32_t event_seq, uint32_t hint_size = 0x4000 );
	PacketBuffer( std::vector< uint8_t > &data, size_t length );

	~PacketBuffer()
	{
	}

	uint16_t get_command()
	{
		return ntohs( ( ( PacketHeader * )&buffer[ 0 ] )->command );
	}

	uint32_t get_event_id()
	{
		return ( ( PacketHeader * )&buffer[ 0 ] )->event_id;
	}

	// Resizes buffer buffer to nearest 1024 when needed
	inline void resize( size_t write_size )
	{
		if( write_position + write_size > buffer.size() )
		{
			buffer.resize( Math::round_up( write_position + write_size + 1, 1024 ) );
		}
	}

	// Write Data
	template < typename T >
	void write( const T i );
	void write_buffer( const uint8_t *buffer, size_t size );
	void write_u8( const uint8_t i );
	void write_u16( const uint16_t i );
	void write_u32( const uint32_t i );
	void write_float( const float f );
	void write_utf8( std::string s );
	void write_sz_utf8( std::string s );
	void write_utf16( std::wstring ws );
	void write_sz_utf16( std::wstring ws );

	// Read Data
	template < typename T >
	T read();

	uint8_t read_u8();
	uint16_t read_u16();
	uint32_t read_u32();
	float_t read_float();
	std::string read_utf8( size_t forced_size = ~0 );
	std::string read_sz_utf8();
	std::wstring read_utf16( size_t forced_size = ~0 );
	std::wstring read_sz_utf16();

	void skip( uint32_t size );

	std::vector< uint8_t > buffer;
	uint32_t write_position;
	uint32_t read_position;
};
*/

//typedef std::shared_ptr< PacketBuffer > sptr_packet;
/*
sptr_packet make_packet( uint16_t command, uint32_t event_id, uint32_t hint_size );
sptr_packet make_packet( sptr_packet request );
void process_networking();
*/