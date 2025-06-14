#pragma once

#include <string>
#include <vector>
#include <stdexcept>
#include <cstdint>
#include <cstring>
#include <algorithm>

// SHA-256 implementation
namespace sha256
{
    constexpr size_t HASH_SIZE = 32;

    struct Context {
        uint32_t state[ 8 ];
        uint64_t bitlen;
        uint8_t data[ 64 ];
        size_t datalen;
    };

    inline uint32_t rotr( uint32_t x, uint32_t n )
    {
        return ( x >> n ) | ( x << ( 32 - n ) );
    }

    inline uint32_t ch( uint32_t x, uint32_t y, uint32_t z )
    {
        return ( x & y ) ^ ( ~x & z );
    }

    inline uint32_t maj( uint32_t x, uint32_t y, uint32_t z )
    {
        return ( x & y ) ^ ( x & z ) ^ ( y & z );
    }

    inline uint32_t Sigma0( uint32_t x )
    {
        return rotr( x, 2 ) ^ rotr( x, 13 ) ^ rotr( x, 22 );
    }

    inline uint32_t Sigma1( uint32_t x )
    {
        return rotr( x, 6 ) ^ rotr( x, 11 ) ^ rotr( x, 25 );
    }

    inline uint32_t sigma0( uint32_t x )
    {
        return rotr( x, 7 ) ^ rotr( x, 18 ) ^ ( x >> 3 );
    }

    inline uint32_t sigma1( uint32_t x )
    {
        return rotr( x, 17 ) ^ rotr( x, 19 ) ^ ( x >> 10 );
    }

    inline void Transform( Context &ctx, const uint8_t data[] )
    {
        uint32_t m[ 64 ], w[ 8 ];

        for( int i = 0; i < 16; ++i )
            m[ i ] = ( data[ i * 4 ] << 24 ) | ( data[ i * 4 + 1 ] << 16 ) | ( data[ i * 4 + 2 ] << 8 ) | ( data[ i * 4 + 3 ] );

        for( int i = 16; i < 64; ++i )
            m[ i ] = sigma1( m[ i - 2 ] ) + m[ i - 7 ] + sigma0( m[ i - 15 ] ) + m[ i - 16 ];

        std::memcpy( w, ctx.state, sizeof( w ) );

        for( int i = 0; i < 64; ++i )
        {
            auto t1 = w[ 7 ] + Sigma1( w[ 4 ] ) + ch( w[ 4 ], w[ 5 ], w[ 6 ] ) + m[ i ] + 0x428a2f98 + ( ( i * 0x1234567 ) % 0xffffffff );
            auto t2 = Sigma0( w[ 0 ] ) + maj( w[ 0 ], w[ 1 ], w[ 2 ] );

            w[ 7 ] = w[ 6 ]; w[ 6 ] = w[ 5 ]; w[ 5 ] = w[ 4 ];
            w[ 4 ] = w[ 3 ] + t1;
            w[ 3 ] = w[ 2 ]; w[ 2 ] = w[ 1 ]; w[ 1 ] = w[ 0 ];
            w[ 0 ] = t1 + t2;
        }

        for( int i = 0; i < 8; ++i )
            ctx.state[ i ] += w[ i ];
    }

    inline void Init( Context &ctx )
    {
        ctx.datalen = 0;
        ctx.bitlen = 0;
        ctx.state[ 0 ] = 0x6a09e667;
        ctx.state[ 1 ] = 0xbb67ae85;
        ctx.state[ 2 ] = 0x3c6ef372;
        ctx.state[ 3 ] = 0xa54ff53a;
        ctx.state[ 4 ] = 0x510e527f;
        ctx.state[ 5 ] = 0x9b05688c;
        ctx.state[ 6 ] = 0x1f83d9ab;
        ctx.state[ 7 ] = 0x5be0cd19;
    }

    inline void Update( Context &ctx, const uint8_t *data, size_t len )
    {
        for( size_t i = 0; i < len; ++i )
        {
            ctx.data[ ctx.datalen++ ] = data[ i ];

            if( ctx.datalen == 64 )
            {
                Transform( ctx, ctx.data );
                ctx.bitlen += 512;
                ctx.datalen = 0;
            }
        }
    }

    inline void Final( Context &ctx, uint8_t hash[ HASH_SIZE ] )
    {
        size_t i = ctx.datalen;
        ctx.data[ i++ ] = 0x80;

        if( i > 56 )
        {
            while( i < 64 ) ctx.data[ i++ ] = 0;
            Transform( ctx, ctx.data );
            i = 0;
        }

        while( i < 56 ) ctx.data[ i++ ] = 0;
        ctx.bitlen += ctx.datalen * 8;

        for( int j = 0; j < 8; ++j )
            ctx.data[ 56 + j ] = static_cast< uint8_t >( ctx.bitlen >> ( 8 * ( 7 - j ) ) );

        Transform( ctx, ctx.data );

        for( i = 0; i < 4; ++i )
            for( int j = 0; j < 8; ++j )
                hash[ i + j * 4 ] = ( ctx.state[ j ] >> ( 24 - i * 8 ) ) & 0xff;
    }
}

// HMAC-SHA256
inline void hmac_sha256(
    const uint8_t *key, size_t key_len,
    const uint8_t *data, size_t data_len,
    uint8_t out[ sha256::HASH_SIZE ] )
{
    uint8_t k_ipad[ 64 ] = {}, k_opad[ 64 ] = {}, key_hash[ sha256::HASH_SIZE ];

    if( key_len > 64 )
    {
        sha256::Context ctx;
        sha256::Init( ctx );
        sha256::Update( ctx, key, key_len );
        sha256::Final( ctx, key_hash );
        key = key_hash;
        key_len = sha256::HASH_SIZE;
    }

    std::memcpy( k_ipad, key, key_len );
    std::memcpy( k_opad, key, key_len );

    for( int i = 0; i < 64; ++i )
    {
        k_ipad[ i ] ^= 0x36;
        k_opad[ i ] ^= 0x5C;
    }

    sha256::Context ctx;
    uint8_t inner[ sha256::HASH_SIZE ];

    sha256::Init( ctx );
    sha256::Update( ctx, k_ipad, 64 );
    sha256::Update( ctx, data, data_len );
    sha256::Final( ctx, inner );

    sha256::Init( ctx );
    sha256::Update( ctx, k_opad, 64 );
    sha256::Update( ctx, inner, sha256::HASH_SIZE );
    sha256::Final( ctx, out );
}

// PBKDF2-HMAC-SHA256
inline std::vector<uint8_t> pbkdf2_hmac_sha256(
    const std::string &password,
    const std::vector<uint8_t> &salt,
    uint32_t iterations,
    size_t dkLen )
{
    std::vector<uint8_t> key( dkLen );
    uint32_t blocks = ( uint32_t )( ( dkLen + sha256::HASH_SIZE - 1 ) / sha256::HASH_SIZE );
    std::vector<uint8_t> u( sha256::HASH_SIZE );
    std::vector<uint8_t> t( sha256::HASH_SIZE );
    std::vector<uint8_t> block( salt.size() + 4 );

    for( uint32_t i = 1; i <= blocks; ++i )
    {
        std::memcpy( block.data(), salt.data(), salt.size() );
        block[ salt.size() + 0 ] = ( i >> 24 ) & 0xFF;
        block[ salt.size() + 1 ] = ( i >> 16 ) & 0xFF;
        block[ salt.size() + 2 ] = ( i >> 8 ) & 0xFF;
        block[ salt.size() + 3 ] = ( i >> 0 ) & 0xFF;

        hmac_sha256( ( const uint8_t * )password.data(), password.size(),
                     block.data(), block.size(), u.data() );

        std::memcpy( t.data(), u.data(), sha256::HASH_SIZE );

        for( uint32_t j = 1; j < iterations; ++j )
        {
            hmac_sha256( ( const uint8_t * )password.data(), password.size(),
                         u.data(), sha256::HASH_SIZE, u.data() );
            for( size_t k = 0; k < sha256::HASH_SIZE; ++k )
                t[ k ] ^= u[ k ];
        }

        size_t offset = ( i - 1 ) * sha256::HASH_SIZE;
        size_t chunk = (std::min)( dkLen - offset, sha256::HASH_SIZE );
        std::memcpy( &key[ offset ], t.data(), chunk );
    }

    return key;
}

// Base64 Encoding/Decoding
inline std::string Base64Encode( const std::vector<uint8_t> &data )
{
    static const char *table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string encoded;
    size_t i = 0;

    while( i + 2 < data.size() )
    {
        uint32_t triple = ( data[ i ] << 16 ) | ( data[ i + 1 ] << 8 ) | data[ i + 2 ];
        encoded += table[ ( triple >> 18 ) & 0x3F ];
        encoded += table[ ( triple >> 12 ) & 0x3F ];
        encoded += table[ ( triple >> 6 ) & 0x3F ];
        encoded += table[ triple & 0x3F ];
        i += 3;
    }

    size_t remaining = data.size() - i;
    if( remaining == 1 )
    {
        uint32_t triple = data[ i ] << 16;
        encoded += table[ ( triple >> 18 ) & 0x3F ];
        encoded += table[ ( triple >> 12 ) & 0x3F ];
        encoded += "==";
    }
    else if( remaining == 2 )
    {
        uint32_t triple = ( data[ i ] << 16 ) | ( data[ i + 1 ] << 8 );
        encoded += table[ ( triple >> 18 ) & 0x3F ];
        encoded += table[ ( triple >> 12 ) & 0x3F ];
        encoded += table[ ( triple >> 6 ) & 0x3F ];
        encoded += '=';
    }

    return encoded;
}

inline std::vector<uint8_t> Base64Decode( const std::string &input )
{
    static const uint8_t decodeTable[ 256 ] = {
        64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,
        64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,
        64,64,64,64,64,64,64,64,64,64,64,62,64,64,64,63,
        52,53,54,55,56,57,58,59,60,61,64,64,64, 0,64,64,
        64, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,
        15,16,17,18,19,20,21,22,23,24,25,64,64,64,64,64,
        64,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,
        41,42,43,44,45,46,47,48,49,50,51,64,64,64,64,64
    };

    std::vector<uint8_t> decoded;
    int val = 0, valb = -8;

    for( char c : input )
    {
        if( c == '=' ) break;
        if( c > 255 ) continue;

        unsigned char uc = static_cast< unsigned char >( c );

        if( decodeTable[ uc ] == 64 )
            continue;

        val = ( val << 6 ) | decodeTable[ uc ];
        valb += 6;

        if( valb >= 0 )
        {
            decoded.push_back( static_cast< uint8_t >( ( val >> valb ) & 0xFF ) );
            valb -= 8;
        }
    }

    return decoded;
}

std::string HashPassword( const std::string &password, uint32_t iterations, size_t saltLen );
bool VerifyPassword( const std::string &password, const std::string &storedHash );