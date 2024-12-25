#pragma once

#include <string>
#include <vector>
#include <span>

// This class is based on the games Encryptor class,
// and is a wrapper around the rijndael ECB implementation.
//
// Normally CoN would generate a random symmetric key for each user,
// but for the sake of simplicity we will just use the games default key,
// since we have nothing to hide.

class RealmCrypt {
private:
	// Byte array of dlfk qs';r+t iqe4t9ueerjKDJ wdaj
	const static inline std::vector< uint8_t > default_sym_key =
	{
		0x64, 0x6c,	0x66, 0x6b, 0x20, 0x71, 0x73, 0x27,
		0x3b, 0x72, 0x2b, 0x74, 0x20, 0x69, 0x71, 0x65,
		0x34, 0x74, 0x39, 0x75, 0x65, 0x65, 0x72, 0x6a,
		0x4b, 0x44, 0x4a, 0x20, 0x77, 0x64, 0x61, 0x6a
	};

public:
	RealmCrypt();

	// Generate a new symmetric key for the user.
	static std::vector< uint8_t > generateSymmetricKey( void );
	static std::vector< uint8_t > getSymmetricKey( void );

	// Encrypt and decrypt strings.
	static std::string encryptString( std::string &input );
	static std::string decryptString( std::string &input );
	static std::wstring encryptString( std::wstring &input );
	static std::wstring decryptString( std::wstring &input );

	// Encrypt and decrypt byte arrays.
	static std::vector< uint8_t > encryptSymmetric( std::vector< const uint8_t > &input );
	static std::vector< uint8_t > decryptSymmetric( std::vector< const uint8_t > &input );
	static std::vector< uint8_t > encryptSymmetric( std::span< const uint8_t > input );
	static std::vector< uint8_t > decryptSymmetric( std::span< const uint8_t > input );

	// Test to make sure the encryption and decryption works.
	void test();

	// Initializer state for srand.
	static bool ms_initialized;
};


/*class Encryptor {
private:
	// "dlfk qs';r+t iqe4t9ueerjKDJ wdaj";
	const static inline std::vector< uint8_t > default_sym_key =
	{
		0x64, 0x6c,	0x66, 0x6b, 0x20, 0x71, 0x73, 0x27,
		0x3b, 0x72, 0x2b, 0x74, 0x20, 0x69, 0x71, 0x65,
		0x34, 0x74, 0x39, 0x75, 0x65, 0x65, 0x72, 0x6a,
		0x4b, 0x44, 0x4a, 0x20, 0x77, 0x64, 0x61, 0x6a
	};

public:
	Encryptor();

	std::vector< uint8_t > generateSymmetricKey( void );

	std::string encryptString( std::string &input );
	std::string decryptString( std::string &input );
	std::wstring encryptString( std::wstring &input );
	std::wstring decryptString( std::wstring &input );

	std::vector< uint8_t > encryptSymmetric( std::vector< const uint8_t > &input );
	std::vector< uint8_t > decryptSymmetric( std::vector< const uint8_t > &input );

	std::vector< uint8_t > encryptSymmetric( std::span< const uint8_t > input );
	std::vector< uint8_t > decryptSymmetric( std::span< const uint8_t > input );

	void setSymmetricKey( const std::vector< uint8_t > &input );
	std::vector< uint8_t > getSymmetricKey( void ) const;

	void test();

	std::vector< uint8_t > m_symKey;

	static bool ms_initialized;
};*/

