/*
#include "../global_define.h"

PacketBuffer::PacketBuffer( uint16_t command, uint32_t event_seq, uint32_t hint_size )
{
	if( hint_size < PACKET_HEADER_SIZE ) hint_size = PACKET_HEADER_SIZE;
	buffer.resize( Math::round_up( hint_size, 0x02 ) );

	write_position = PACKET_HEADER_SIZE;
	read_position = PACKET_HEADER_SIZE;

	*( PacketHeader * )&buffer[ 0 ] = { PACKET_HEADER_SIZE, htons( command ), event_seq, 0 };
}

PacketBuffer::PacketBuffer( std::vector< uint8_t > &data, size_t length )
{
	buffer.resize( length );
	std::copy( data.begin(), data.begin() + length, buffer.begin() );

	write_position = length;
	read_position = PACKET_HEADER_SIZE;
}

template < typename T >
void PacketBuffer::write( const T i )
{
	if( ( write_position + sizeof( T ) ) >= buffer.size() )
	{
		buffer.resize( Math::round_up( ( write_position + sizeof( T ) ) + 1, 1024 ) );
	}

	memcpy( &buffer.data()[ write_position ], &i, sizeof( i ) );
	write_position += sizeof( i );
}

void PacketBuffer::skip( const uint32_t size )
{
	write_position += size;
	read_position += size;
}

void PacketBuffer::write_buffer( const uint8_t *in_buffer, size_t size )
{
	resize( size );

	memcpy( &buffer.data()[ write_position ], &in_buffer[ 0 ], size );
	write_position += size;
}

void PacketBuffer::write_u8( const uint8_t i )
{
	write<uint8_t>( i );
}

void PacketBuffer::write_u16( const uint16_t i )
{
	write<uint16_t>( i );
}

void PacketBuffer::write_u32( const uint32_t i )
{
	write<uint32_t>( i );
}

void PacketBuffer::write_float( const float f )
{
	write<float_t>( f );
}

void PacketBuffer::write_utf8( std::string s )
{
	auto length = static_cast< int16_t >( s.length() );
	write_u32( length );

	if( ( write_position + length ) >= buffer.size() )
	{
		buffer.resize( Math::round_up( ( write_position + length ) + 1, 1024 ) );
	}

	memcpy( &buffer.data()[ write_position ], &s.data()[ 0 ], s.length() );
	write_position += length;
}

void PacketBuffer::write_sz_utf8( std::string s )
{
	auto length = static_cast< int16_t >( s.length() );

	if( ( write_position + length ) >= buffer.size() )
	{
		buffer.resize( Math::round_up( ( write_position + length ) + 1, 1024 ) );
	}

	memcpy( &buffer.data()[ write_position ], &s.data()[ 0 ], s.length() );
	write_position += length;

	// Null-Terminator
	write_u8( 0 );
}

void PacketBuffer::write_utf16( std::wstring ws )
{
	auto length = ws.length();
	write_u32( length );

	length = length * sizeof( std::wstring::value_type );

	if( ( write_position + length ) >= buffer.size() )
	{
		buffer.resize( Math::round_up( ( write_position + length ) + 1, 1024 ) );
	}

	memcpy( &buffer.data()[ write_position ], &ws.data()[ 0 ], length );
	write_position += length;

	write_u16( 0 );
}

void PacketBuffer::write_sz_utf16( std::wstring ws )
{
	auto length = ws.length() * sizeof( std::wstring::value_type );

	if( ( write_position + length ) >= buffer.size() )
	{
		buffer.resize( Math::round_up( ( write_position + length ) + 1, 1024 ) );
	}

	memcpy( &buffer.data()[ write_position ], &ws.data()[ 0 ], length );
	write_position += length;

	// Null-Terminator
	write_u16( 0 );
}

template<typename T>
T PacketBuffer::read()
{
	T ret = *( T * )&buffer.data()[ read_position ];
	read_position += sizeof( T );

	return ret;
}

uint8_t PacketBuffer::read_u8()
{
	return read< uint8_t >();
}

uint16_t PacketBuffer::read_u16()
{
	return read< uint16_t >();
}

uint32_t PacketBuffer::read_u32()
{
	return read< uint32_t >();
}

float_t PacketBuffer::read_float()
{
	return read< float_t >();
}

std::string PacketBuffer::read_utf8( size_t forced_size )
{
	uint32_t length = read_u32();

	if( forced_size != ~0 )
	{
		length = forced_size;
	}

	auto l = buffer.begin() + read_position;
	auto r = buffer.begin() + ( read_position += length );

	return std::string( l, r );
}

std::string PacketBuffer::read_sz_utf8()
{
	std::string str;

	while( auto c = read_u8() )
	{
		str.push_back( c );
		read_position++;
	}

	read_position++;
	return str;
}

std::wstring PacketBuffer::read_utf16( size_t forced_size )
{
	//std::wstring wstr; wstr.resize( length + 1 );
	//memcpy( &wstr[ 0 ], &buffer.data()[ write_position ], length * sizeof( wchar_t ) );
	//write_position += length * sizeof( wchar_t );

	uint32_t length = read_u32() * 2;

	if( forced_size != ~0 )
	{
		length = forced_size * 2;
	}

	auto l = buffer.begin() + read_position;
	auto r = buffer.begin() + ( read_position += length );

	return std::wstring( l, r );
}

std::wstring PacketBuffer::read_sz_utf16()
{
	std::wstring wstr;

	while( auto c = read_u16() )
	{
		wstr.push_back( c );
		read_position += 2;
	}

	read_position += 2;
	return wstr;
}

sptr_packet make_packet( uint16_t command, uint32_t event_id, uint32_t hint_size )
{
	auto p = std::make_shared< PacketBuffer >( command, event_id, hint_size );
	return p;
}

sptr_packet make_packet( sptr_packet request )
{
	auto p = std::make_shared< PacketBuffer >( request->get_command(), request->get_event_id() );
	return p;

}

void process_networking()
{
	sptr_socket s;
	sptr_packet p;

	if( !socket_manager->get_message( s, p ) )
	{
		return;
	}

	switch( s->channel )
	{
		case RealmChannelType::GATEWAY:
		{
			Protocol::Gateway::process_request( s, p );
			break;
		}

		case RealmChannelType::GAME:
		{
			Protocol::Game::process_request( s, p );
			break;
		}
	}
}
*/