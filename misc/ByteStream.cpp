
#include "ByteStream.h"


ByteStream::ByteStream( const std::vector< uint8_t > &data )
{
	this->data = data;
	this->write_position = 0;
}

ByteStream::ByteStream( const std::string &data )
{
	this->data = std::vector< uint8_t >( data.begin(), data.end() );
	this->write_position = 0;
}

ByteStream::ByteStream( const uint8_t *data, size_t length )
{
	this->data = std::vector< uint8_t >( data, data + length );
	this->write_position = 0;
}

ByteStream::ByteStream( size_t length )
{
	this->data = std::vector< uint8_t >( length, 0 );
	this->write_position = 0;
}

ByteStream::ByteStream()
{
	this->write_position = 0;
}

ByteStream::~ByteStream()
{
}

void ByteStream::resize( size_t size )
{
	data.resize( size );
}

void ByteStream::shrink_to_fit()
{
	data.shrink_to_fit();
}

template < typename T >
void ByteStream::write( T value )
{
	write_bytes( ( uint8_t * )&value, sizeof( T ) );
}

template < typename T >
T ByteStream::read()
{
	T value = *( T * )&data[ write_position ];
	write_position += sizeof( T );

	return value;
}

void ByteStream::write_utf8( const std::string &value )
{
	write_bytes( std::vector< uint8_t >( value.begin(), value.end() ) );
}

void ByteStream::write_utf16( const std::wstring &value )
{
	std::vector< uint8_t > utf16;
	for( auto c : value )
	{
		utf16.push_back( c & 0xFF );
		utf16.push_back( ( c >> 8 ) & 0xFF );
	}

	write_bytes( utf16 );
}

void ByteStream::write_sz_utf8( const std::string &value )
{
	write_utf8( value );
	write< uint8_t >( 0 );
}

void ByteStream::write_sz_utf16( const std::wstring &value )
{
	write_utf16( value );
	write<uint16_t>( 0 );
}

uint8_t ByteStream::read_u8()
{
	return read< uint8_t >();
}

uint16_t ByteStream::read_u16()
{
	return read< uint16_t >();
}

uint32_t ByteStream::read_u32()
{
	return read< uint32_t >();
}

int8_t ByteStream::read_i8()
{
	return read< int8_t >();
}

int16_t ByteStream::read_i16()
{
	return read< int16_t >();
}

int32_t ByteStream::read_i32()
{
	return read< int32_t >();
}

float_t ByteStream::read_f32()
{
	return read< float_t >();
}

std::string ByteStream::read_utf8()
{
	uint32_t length = read_u32();
	std::string value;
	for( size_t i = 0; i < length; i++ )
	{
		value.push_back( data[ write_position + i ] );
	}

	write_position += length;

	return value;
}

std::wstring ByteStream::read_utf16()
{
	std::wstring value;
	uint32_t length = read_u32() * 2;

	for( size_t i = 0; i < length; i += 2 )
	{
		value.push_back( data[ write_position + i ] | ( data[ write_position + i + 1 ] << 8 ) );
	}

	write_position += length;

	return value;
}

std::string ByteStream::read_sz_utf8()
{
	std::string value;
	while( data[ write_position ] != 0 )
	{
		value.push_back( data[ write_position ] );
		write_position++;
	}

	write_position++;

	return value;
}

std::wstring ByteStream::read_sz_utf16()
{
	std::wstring value;
	while( data[ write_position ] != 0 || data[ write_position + 1 ] != 0 )
	{
		value.push_back( data[ write_position ] | ( data[ write_position + 1 ] << 8 ) );
		write_position += 2;
	}

	write_position += 2;

	return value;
}

void ByteStream::write_bytes( const std::vector< uint8_t > &value )
{
	std::copy( value.begin(), value.end(), std::back_inserter( data ) );
	write_position += value.size();
}

void ByteStream::write_bytes( const uint8_t *value, size_t length )
{
	std::copy( value, value + length, std::back_inserter( data ) );
	write_position += length;
}

std::vector<uint8_t> ByteStream::read_bytes( size_t length )
{
	std::vector<uint8_t> value( length, 0 );

	std::copy( data.begin() + write_position, data.begin() + write_position + length, value.begin() );

	write_position += length;

	return value;
}

std::vector<uint8_t> ByteStream::get_data() const
{
	return data;
}

size_t ByteStream::get_length() const
{
	return data.size();
}

void ByteStream::set_write_position( size_t write_position )
{
	if( write_position > data.size() )
	{
		write_position = data.size();
	}

	this->write_position = write_position;
}

size_t ByteStream::get_write_position() const
{
	return this->write_position;
}

void ByteStream::write_u8( uint8_t value )
{
	write< uint8_t >( value );
}

void ByteStream::write_u16( uint16_t value )
{
	write< uint16_t >( value );
}

void ByteStream::write_u32( uint32_t value )
{
	write< uint32_t >( value );
}

void ByteStream::write_i8( int8_t value )
{
	write< int8_t >( value );
}

void ByteStream::write_i16( int16_t value )
{
	write< int16_t >( value );
}

void ByteStream::write_i32( int32_t value )
{
	write< int32_t >( value );
}

void ByteStream::write_f32( float_t value )
{
	write< float_t >( value );
}
