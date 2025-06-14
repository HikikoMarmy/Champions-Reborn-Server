
#include "RealmCrypt.h"
#include "../Common/Utility.h"

RealmCrypt::RealmCrypt()
{
	std::srand( static_cast< unsigned >( std::time( nullptr ) ) );
}

std::vector< uint8_t > RealmCrypt::generateSymmetricKey( void )
{
	constexpr size_t KEY_LENGTH = 32;

	std::vector< uint8_t > keyData( KEY_LENGTH, 0 );

	// Generate 32 random bytes
	for( size_t i = 0; i < KEY_LENGTH; ++i )
	{
		keyData[ i ] = static_cast< uint8_t >( std::rand() % 255 );
	}

	return keyData;
}

std::vector<uint8_t> RealmCrypt::getSymmetricKey( void )
{
	return default_sym_key;
}

std::string RealmCrypt::encryptString( std::string &input )
{
	if( input.size() % 16 != 0 )
	{
		input.append( 16 - ( input.size() % 16 ), '\0' );
	}

	rijndael aes;

	auto result = aes.EncryptECB(
		reinterpret_cast< const uint8_t * >( input.c_str() ),
		static_cast< uint32_t >( input.size() ),
		default_sym_key.data()
	);

	return std::string( reinterpret_cast< const char * >( result ), input.size() );
}

std::string RealmCrypt::decryptString( std::string &input )
{
	if( input.size() % 16 != 0 )
	{
		input.append( 16 - ( input.size() % 16 ), '\0' );
	}

	rijndael aes;

	auto result = aes.DecryptECB( reinterpret_cast< const uint8_t * >(
		input.c_str() ),
		static_cast< uint32_t >( input.size() ),
		default_sym_key.data()
	);

	return std::string( reinterpret_cast< const char * >( result ), input.size() );
}

std::vector<uint8_t> RealmCrypt::encryptString( const std::wstring &input )
{
	// Convert UTF-16 string to raw bytes
	std::vector<uint8_t> utf16Bytes;
	utf16Bytes.reserve( input.size() * 2 );
	for( wchar_t ch : input )
	{
		utf16Bytes.push_back( static_cast< uint8_t >( ch & 0xFF ) );
		utf16Bytes.push_back( static_cast< uint8_t >( ( ch >> 8 ) & 0xFF ) );
	}

	// Pad to nearest 16 bytes
	if( utf16Bytes.size() % 16 != 0 )
	{
		utf16Bytes.resize( ( utf16Bytes.size() / 16 + 1 ) * 16, 0 );
	}

	// Encrypt using AES ECB
	rijndael aes;
	auto encrypted = aes.EncryptECB(
		reinterpret_cast< const uint8_t * >( utf16Bytes.data() ),
		static_cast< uint32_t >( utf16Bytes.size() ),
		default_sym_key.data()
	);

	// Return the raw encrypted bytes
	return std::vector<uint8_t>( encrypted, encrypted + utf16Bytes.size() );
}

std::wstring RealmCrypt::decryptString( std::vector<uint8_t> &input )
{
	// Ensure input is a multiple of 16 bytes
	if( input.size() % 16 != 0 )
	{
		input.resize( ( input.size() / 16 + 1 ) * 16, 0 );
	}

	rijndael aes;

	auto result = aes.DecryptECB(
		reinterpret_cast< const uint8_t * >( input.data() ),
		static_cast< uint32_t >( input.size() ),
		default_sym_key.data()
	);

	// Convert decrypted bytes back into a wstring
	std::wstring output;
	output.reserve( input.size() / 2 );

	for( size_t i = 0; i + 1 < input.size(); i += 2 )
	{
		uint16_t ch = static_cast< uint16_t >( input[ i ] | ( input[ i + 1 ] << 8 ) );
		if( ch == 0 ) break; // Optional: stop at null terminator
		output.push_back( static_cast< wchar_t >( ch ) );
	}

	return output;
}

std::vector< uint8_t > RealmCrypt::encryptSymmetric( std::span< const uint8_t > input )
{
	if( input.size() % 16 != 0 )
	{
		std::vector< uint8_t > paddedInput( input.begin(), input.end() );
		paddedInput.resize( ( ( input.size() / 16 ) + 1 ) * 16, 0 );
		input = paddedInput;
	}

	rijndael aes;

	auto result = aes.EncryptECB( reinterpret_cast< const uint8_t * >(
		input.data() ),
		static_cast< uint32_t >( input.size() ),
		default_sym_key.data()
	);

	return std::vector< uint8_t >( result, result + input.size() );
}

std::vector< uint8_t > RealmCrypt::decryptSymmetric( std::span< const uint8_t > input )
{
	if( input.size() % 16 != 0 )
	{
		std::vector< uint8_t > paddedInput( input.begin(), input.end() );
		paddedInput.resize( ( ( input.size() / 16 ) + 1 ) * 16, 0 );
		input = paddedInput;
	}

	rijndael aes;

	auto result = aes.DecryptECB( reinterpret_cast< const uint8_t * >(
		input.data() ),
		static_cast< uint32_t >( input.size() ),
		default_sym_key.data()
	);

	return std::vector< uint8_t >( result, result + input.size() );
}