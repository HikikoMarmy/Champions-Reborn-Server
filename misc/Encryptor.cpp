#include "Encryptor.h"
#include <ctime>
#include <array>

#include "AES.h"

bool Encryptor::ms_initialized = false;

Encryptor::Encryptor()
{
	// Initialize the private key
	m_privateKey = ""; // Default initialization

	// Initialize basic_str_b
	basic_str_b = "";

	// Initialize the symmetric key
	m_symmetricKey.assign( default_sym_key );

	// Static initialization logic
	if( !ms_initialized )
	{
		ms_initialized = true;
		std::srand( static_cast< unsigned >( std::time( nullptr ) ) );
		Encryptor::test();
	}
}

std::string Encryptor::generateSymmetricKey( void )
{
	constexpr size_t KEY_LENGTH = 32;

	std::array<unsigned char, KEY_LENGTH> keyData{ 0 };

	// Generate 32 random bytes
	for( size_t i = 0; i < KEY_LENGTH; ++i )
	{
		keyData[ i ] = static_cast< unsigned char >( rand() % 255 );
	}
	
	// Replace the symmetric key with the generated key
	m_symmetricKey.assign( reinterpret_cast< char * >( keyData.data() ), KEY_LENGTH );

	return m_symmetricKey;
}

std::string Encryptor::generatePrivateSymKey( void )
{
	constexpr size_t KEY_LENGTH = 32;

	std::array<unsigned char, KEY_LENGTH> keyData{ 0 };

	// Generate 32 random bytes
	for( size_t i = 0; i < KEY_LENGTH; ++i )
	{
		keyData[ i ] = static_cast< unsigned char >( rand() % 255 );
	}

	// Replace the symmetric key with the generated key
	m_privateKey.assign( reinterpret_cast< char * >( keyData.data() ), KEY_LENGTH );

	// Print the private key as bytes
	printf( "Private Sym Key: " );
	for( auto c : m_privateKey )
	{
		printf( "%02X", (uint8_t)c );
	}
	printf( "\n" );

	// Encrypt the private key
	AES aes( AESKeyLength::AES_128 );

	auto c = aes.EncryptECB(
		reinterpret_cast< const uint8_t * >( m_privateKey.c_str() ),
		m_privateKey.size(),
		reinterpret_cast< const uint8_t * >( default_public_key.c_str() ) );

	m_encryptedPrivateKey = std::string( reinterpret_cast< const char * >( c ), m_privateKey.size() );

	// Print the encrypted key as bytes
	printf( "Encrypted Sym Key: " );
	for( auto c : m_encryptedPrivateKey )
	{
		printf( "%02X", ( uint8_t )c );
	}
	printf( "\n" );

	return m_encryptedPrivateKey;
}

std::string Encryptor::encryptSymmetric( const std::string &input )
{
	AES aes( AESKeyLength::AES_128 );

	auto result = aes.EncryptECB( reinterpret_cast< const uint8_t * >( input.c_str() ), input.size(), reinterpret_cast< const uint8_t * >( m_privateKey.c_str() ) );

	return std::string( reinterpret_cast< const char * >( result ), input.size() );
}

std::string Encryptor::decryptSymmetric( const std::string &input )
{
	AES aes( AESKeyLength::AES_128 );

	auto result = aes.DecryptECB( reinterpret_cast< const uint8_t * >( input.c_str() ), input.size(), reinterpret_cast< const uint8_t * >( m_privateKey.c_str() ) );

	return std::string( reinterpret_cast< const char * >( result ), input.size() );
}

void Encryptor::test()
{
	/*std::string inputStr = "HelloWorld";        // Input string to encrypt and decrypt
	std::string intermediateEncryptedStr;      // Encrypted intermediate result
	std::string intermediateDecryptedStr;      // Decrypted intermediate result

	// Generate symmetric key
	std::string symmetricKey;
	generateSymmetricKey( symmetricKey );

	// Encrypt the input string using the symmetric key
	encryptor.encryptSymmetric( intermediateEncryptedStr, inputStr );

	// Log intermediate encryption result
	std::cout << "Encrypted string: " << intermediateEncryptedStr << std::endl;

	// Decrypt the encrypted string using the symmetric key
	encryptor.decryptSymmetric( intermediateDecryptedStr, intermediateEncryptedStr );

	// Log final decryption result
	std::cout << "Decrypted string: " << intermediateDecryptedStr << std::endl;

	// Check if decryption matches the original input
	if( inputStr == intermediateDecryptedStr )
	{
		std::cout << "Test passed: Decryption matches original input." << std::endl;
	}
	else
	{
		std::cout << "Test failed: Decryption does not match original input." << std::endl;
	}*/
}

int decryptBuffer( const uint8_t *input, int32_t dataSize, uint8_t *output, const uint8_t *symKey )
{
	if( dataSize <= 0 )
	{
		return false; // No data to decrypt
	}

	AES aes( AESKeyLength::AES_128 );
	output = aes.DecryptECB( input, dataSize, symKey );

	return true;
}
