#pragma once

#include <string>
#include <vector>
#include <span>

#include "rijndael.h"

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
	static std::vector<uint8_t> encryptString( const std::wstring &input );
	static std::wstring decryptString( std::vector<uint8_t> &input );

	// Encrypt and decrypt byte arrays.
	static std::vector< uint8_t > encryptSymmetric( std::span< const uint8_t > input );
	static std::vector< uint8_t > decryptSymmetric( std::span< const uint8_t > input );
};