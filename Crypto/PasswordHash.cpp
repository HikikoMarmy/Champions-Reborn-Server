#include <random>
#include <sstream>
#include <iomanip>
#include <stdexcept>

#include "PasswordHash.h"

std::string HexDump( const std::vector<uint8_t> &bytes )
{
    std::ostringstream oss;
    for( auto b : bytes )
        oss << std::hex << std::setfill( '0' ) << std::setw( 2 ) << ( int )b;
    return oss.str();
}

std::string HashPassword( const std::string &password, uint32_t iterations, size_t saltLen )
{
    std::vector<uint8_t> salt( saltLen );

    std::random_device rd;
    std::mt19937 rng( rd() );
    std::uniform_int_distribution<int32_t> dist( 0, 255 );

    for( auto &b : salt ) b = static_cast< int32_t >( dist( rng ) );

    auto derived = pbkdf2_hmac_sha256( password, salt, iterations, 32 );

    return "pbkdf2$" + std::to_string( iterations ) + "$" +
        Base64Encode( salt ) + "$" +
        Base64Encode( derived );

}

bool VerifyPassword( const std::string &password, const std::string &storedHash )
{
    auto parts = std::vector<std::string>();
    std::stringstream ss( storedHash );
    std::string token;

    while( std::getline( ss, token, '$' ) )
        parts.push_back( token );

    if( parts.size() != 4 || parts[ 0 ] != "pbkdf2" )
        throw std::runtime_error( "Invalid hash format" );

    uint32_t iterations = std::stoul( parts[ 1 ] );
    std::vector<uint8_t> salt = Base64Decode( parts[ 2 ] );
    std::vector<uint8_t> expected = Base64Decode( parts[ 3 ] );

    auto derived = pbkdf2_hmac_sha256( password, salt, iterations, expected.size() );

    return derived == expected;
}
