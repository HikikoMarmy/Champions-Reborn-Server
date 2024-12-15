
#pragma once

#include <string>

int decryptBuffer( const uint8_t *input, int64_t dataSize, uint8_t *output, const uint8_t *symKey );

class Encryptor
{
public:
	const static inline std::string default_public_key = "25B946EBC0B361734A63910E1FF3C9E1";
	const static inline std::string default_sym_key = "dlfk qs';r+t iqe4t9ueerjKDJ wdaj";

    Encryptor(); // Constructor

    std::string generateSymmetricKey( void );
	std::string generatePrivateSymKey( void );

	std::string encryptSymmetric( const std::string &input );
	std::string decryptSymmetric( const std::string &input );

	void setSymmetricKey( const std::string &key );
	std::string getSymmetricKey( void ) const;

	void setPublicKey( const std::string &key );
	std::string getPublicKey( void ) const;

	void setPrivateKey( const std::string &key );
	std::string getPrivateKey( void ) const;

    static void test();

	std::string m_privateKey, m_encryptedPrivateKey;
    std::string m_symmetricKey;
    std::string basic_str_b;

    static bool ms_initialized;
};

