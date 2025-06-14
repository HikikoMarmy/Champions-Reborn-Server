#include <codecvt>
#include "ByteStream.h"
#include <span>

ByteBuffer::ByteBuffer( const std::vector< uint8_t > &data )
{
	this->m_buffer = data;
	this->m_position = 0;
}

ByteBuffer::ByteBuffer( const std::string &data )
{
	this->m_buffer = std::vector< uint8_t >( data.begin(), data.end() );
	this->m_position = 0;
}

ByteBuffer::ByteBuffer( const uint8_t *data, uint32_t length )
{
	this->m_buffer = std::vector< uint8_t >( data, data + length );
	this->m_position = 0;
}

ByteBuffer::ByteBuffer( uint32_t length )
{
	this->m_buffer = std::vector< uint8_t >( length, 0 );
	this->m_position = 0;
}

ByteBuffer::ByteBuffer()
{
	this->m_position = 0;
}

ByteBuffer::~ByteBuffer()
{
}

void ByteBuffer::resize( uint32_t size )
{
	m_buffer.resize( size );
}

void ByteBuffer::shrink_to_fit()
{
	m_buffer.shrink_to_fit();
}

template < typename T >
void ByteBuffer::write( T value )
{
	write_bytes( ( uint8_t * )&value, sizeof( T ) );
}

template < typename T >
T ByteBuffer::read()
{
	if( m_position >= m_buffer.size() )
	{
		return (T)0;
	}

	T value = *( T * )&m_buffer[ m_position ];
	m_position += sizeof( T );

	return value;
}

void ByteBuffer::write_utf8( const std::string &str, std::optional<uint32_t> length )
{
	if( length.has_value() )
	{
		write_u32( length.value() );

		if( length.value() > str.size() )
		{
			write_bytes( std::vector< uint8_t >( str.begin(), str.end() ) );
			write_bytes( std::vector< uint8_t >( length.value() - str.size(), 0 ) );
		}
		else
		{
			write_bytes( std::vector< uint8_t >( str.begin(), str.begin() + length.value() ) );
		}
	}
	else
	{
		write_u32( static_cast< uint32_t >( str.size() ) );
		write_bytes( std::vector< uint8_t >( str.begin(), str.end() ) );
	}
}

void ByteBuffer::write_utf16( const std::wstring &str, std::optional<uint32_t> length )
{
	write_u32( static_cast< uint32_t >( str.size() ) );

	for( wchar_t ch : str )
	{
		uint16_t val = static_cast< uint16_t >( ch );
		write<uint8_t>( val & 0xFF );
		write<uint8_t>( ( val >> 8 ) & 0xFF );
	}
}

void ByteBuffer::write_sz_utf8( const std::string &str, std::optional<uint32_t> length )
{
	if( length )
	{
		write_bytes( std::vector< uint8_t >( str.begin(), str.end() ) );
		write_bytes( std::vector< uint8_t >( length.value() - str.size(), 0 ) );
	}
	else
	{
		write_bytes( std::vector< uint8_t >( str.begin(), str.end() ) );
		write< uint8_t >( 0 );
	}
}

void ByteBuffer::write_sz_utf16( const std::wstring &str, std::optional<uint32_t> length )
{
	for( wchar_t ch : str )
	{
		uint16_t val = static_cast< uint16_t >( ch );
		write<uint8_t>( val & 0xFF );
		write<uint8_t>( ( val >> 8 ) & 0xFF );
	}

	if( length )
	{
		size_t bytesWritten = str.size() * 2;
		size_t totalBytes = length.value();

		if( bytesWritten < totalBytes )
		{
			write_bytes( std::vector<uint8_t>( totalBytes - bytesWritten, 0 ) );
		}
	}
	else
	{
		write<uint16_t>( 0 );
	}
}

void ByteBuffer::write_encrypted_utf8( const std::string &str )
{
	auto encrypted = RealmCrypt::encryptSymmetric( std::vector< uint8_t >( str.begin(), str.end() ) );

	write_u32( static_cast< uint32_t >( encrypted.size() ) + 4 );
	write_u32( static_cast< uint32_t >( str.size() ) );

	write_bytes( encrypted );
}

void ByteBuffer::write_encrypted_utf16( const std::wstring &str )
{
	std::vector< uint8_t > utf16;
	for( auto c : str )
	{
		utf16.push_back( c & 0xFF );
		utf16.push_back( ( c >> 8 ) & 0xFF );
	}

	auto encrypted = RealmCrypt::encryptSymmetric( utf16 );
	uint32_t encryptedLength = static_cast< uint32_t >( encrypted.size() );
	uint32_t decryptedLength = static_cast< uint32_t >( utf16.size() );

	// Correct blockLength: in 2-byte words, including the 4-byte decrypted length
	write_u32( ( encryptedLength + 4 ) / 2 );
	write_u32( decryptedLength );

	write_bytes( encrypted );
}

uint8_t ByteBuffer::read_u8()
{
	return read< uint8_t >();
}

uint16_t ByteBuffer::read_u16()
{
	return read< uint16_t >();
}

uint32_t ByteBuffer::read_u32()
{
	return read< uint32_t >();
}

int8_t ByteBuffer::read_i8()
{
	return read< int8_t >();
}

int16_t ByteBuffer::read_i16()
{
	return read< int16_t >();
}

int32_t ByteBuffer::read_i32()
{
	return read< int32_t >();
}

float_t ByteBuffer::read_f32()
{
	return read< float_t >();
}

std::string ByteBuffer::read_utf8( std::optional<uint32_t> length )
{
	if( !length )
	{
		length = read_u32();
	}

	if( m_position + length.value() > m_buffer.size() )
	{
		throw std::runtime_error( "read_utf8: Attempt to read past end of buffer" );
	}

	std::string value( reinterpret_cast< const char * >( &m_buffer[ m_position ] ), length.value() );
	m_position += length.value();

	return value;
}

std::wstring ByteBuffer::read_utf16( std::optional<uint32_t> length )
{
	if( !length )
	{
		length = read_u32();
	}

	uint32_t byteLength = length.value() * 2;

	if( m_position + byteLength > m_buffer.size() )
	{
		throw std::runtime_error( "read_utf16: Attempt to read past end of buffer" );
	}

	std::wstring value;
	value.reserve( length.value() );

	for( size_t i = 0; i < byteLength; i += 2 )
	{
		uint16_t ch = m_buffer[ m_position + i ] | ( m_buffer[ m_position + i + 1 ] << 8 );
		value.push_back( static_cast< wchar_t >( ch ) );
	}

	m_position += byteLength;
	return value;
}

std::string ByteBuffer::read_sz_utf8()
{
	std::string value;
	while( m_buffer[ m_position ] != 0 )
	{
		value.push_back( m_buffer[ m_position ] );
		m_position++;
	}

	m_position++;

	return value;
}

std::wstring ByteBuffer::read_sz_utf16()
{
	std::wstring value;
	while( m_buffer[ m_position ] != 0 || m_buffer[ m_position + 1 ] != 0 )
	{
		value.push_back( m_buffer[ m_position ] | ( m_buffer[ m_position + 1 ] << 8 ) );
		m_position += 2;
	}

	m_position += 2;

	return value;
}

std::string ByteBuffer::read_encrypted_utf8( bool hasBlockLength )
{
	uint32_t encryptedLength = 0;
	uint32_t decryptedLength = 0;

	if( hasBlockLength )
	{
		uint32_t blockLength = read_u32() * 2;
		decryptedLength = read_u32();
		encryptedLength = blockLength - 4;
	}
	else
	{
		decryptedLength = read_u32();
		encryptedLength = Util::round_up( decryptedLength, 16 );
	}

	std::span< const uint8_t > encryptedBuffer( m_buffer.data() + m_position, encryptedLength );

	m_position += encryptedLength;

	if( decryptedLength == 0 )
	{
		return "";
	}

	// Decrypt the buffer
	std::vector< uint8_t > decryptedBuffer = RealmCrypt::decryptSymmetric( encryptedBuffer );

	std::string result( decryptedBuffer.begin(), decryptedBuffer.end() );

	return result;
}

std::wstring ByteBuffer::read_encrypted_utf16( bool hasBlockLength )
{
	uint32_t encryptedLength = 0;
	uint32_t decryptedLength = 0;

	if( hasBlockLength )
	{
		uint32_t blockLength = read_u32() * 2;
		decryptedLength = read_u32();	// This length is already multiplied by sizeof(wchar_t)
		encryptedLength = blockLength - 4;
	}
	else
	{
		decryptedLength = read_u32();
		encryptedLength = Util::round_up( decryptedLength, 16 );
	}

	std::span< const uint8_t > encryptedBuffer( m_buffer.data() + m_position, encryptedLength );

	m_position += encryptedLength;

	if( decryptedLength == 0 )
	{
		return L"";
	}

	// Decrypt the buffer
	std::vector< uint8_t > decryptedBuffer = RealmCrypt::decryptSymmetric( encryptedBuffer );

	std::wstring result( decryptedLength / 2, L'\0' );
	std::memcpy( result.data(), decryptedBuffer.data(), decryptedLength );

	return result;
}

void ByteBuffer::write_bytes( const std::vector< uint8_t > &value )
{
	std::copy( value.begin(), value.end(), std::back_inserter( m_buffer ) );
	m_position += value.size();
}

void ByteBuffer::write_bytes( const uint8_t *value, uint32_t length )
{
	std::copy( value, value + length, std::back_inserter( m_buffer ) );
	m_position += length;
}

void ByteBuffer::write_encrypted_bytes( const std::vector<uint8_t> &value )
{
	auto encrypted = RealmCrypt::encryptSymmetric( value );

	write_u32( static_cast< uint32_t >( encrypted.size() ) + 4 );
	write_u32( static_cast< uint32_t >( value.size() ) );

	write_bytes( encrypted );
}

std::vector<uint8_t> ByteBuffer::read_bytes( uint32_t length )
{
	std::vector<uint8_t> value( length, 0 );

	std::copy( m_buffer.begin() + m_position, m_buffer.begin() + m_position + length, value.begin() );

	m_position += length;

	return value;
}

std::vector<uint8_t> ByteBuffer::read_encrypted_bytes( uint32_t length )
{
	std::vector< uint8_t > encrypted = read_bytes( length );

	auto decrypted = RealmCrypt::decryptSymmetric( encrypted );

	return decrypted;
}

std::vector<uint8_t> ByteBuffer::get_buffer() const
{
	return m_buffer;
}

size_t ByteBuffer::get_length() const
{
	return m_buffer.size();
}

void ByteBuffer::set_position( size_t where )
{
	if( where > m_buffer.size() )
	{
		where = m_buffer.size();
	}

	this->m_position = where;
}

size_t ByteBuffer::get_position() const
{
	return this->m_position;
}

void ByteBuffer::write_u8( uint8_t value )
{
	write< uint8_t >( value );
}

void ByteBuffer::write_u16( uint16_t value )
{
	write< uint16_t >( value );
}

void ByteBuffer::write_u32( uint32_t value )
{
	write< uint32_t >( value );
}

void ByteBuffer::write_i8( int8_t value )
{
	write< int8_t >( value );
}

void ByteBuffer::write_i16( int16_t value )
{
	write< int16_t >( value );
}

void ByteBuffer::write_i32( int32_t value )
{
	write< int32_t >( value );
}

void ByteBuffer::write_f32( float_t value )
{
	write< float_t >( value );
}
