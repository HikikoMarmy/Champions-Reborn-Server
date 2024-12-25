#include "NorrathCrypt.h"

rijndael::rijndael( const KeyLength keyLength )
{
    switch( keyLength )
    {
        case KeyLength::_128:
            this->Nk = 4;
            this->Nr = 10;
            break;
        case KeyLength::_192:
            this->Nk = 6;
            this->Nr = 12;
            break;
        case KeyLength::_256:
            this->Nk = 8;
            this->Nr = 14;
            break;
    }
}

unsigned char *rijndael::EncryptECB( const unsigned char in[], unsigned int inLen,
                                const unsigned char key[] )
{
    CheckLength( inLen );
    unsigned char *out = new unsigned char[ inLen ];
    unsigned char *roundKeys = new unsigned char[ 4 * Nb * ( Nr + 1 ) ];
    KeyExpansion( key, roundKeys );
    for( unsigned int i = 0; i < inLen; i += blockBytesLen )
    {
        EncryptBlock( in + i, out + i, roundKeys );
    }

    delete[] roundKeys;

    return out;
}

unsigned char *rijndael::DecryptECB( const unsigned char in[], unsigned int inLen,
                                const unsigned char key[] )
{
    CheckLength( inLen );
    unsigned char *out = new unsigned char[ inLen ];
    unsigned char *roundKeys = new unsigned char[ 4 * Nb * ( Nr + 1 ) ];
    KeyExpansion( key, roundKeys );

    for( unsigned int i = 0; i < inLen; i += blockBytesLen )
    {
        DecryptBlock( in + i, out + i, roundKeys );
    }

    delete[] roundKeys;

    return out;
}

void rijndael::CheckLength( unsigned int len )
{
    if( len % blockBytesLen != 0 )
    {
        throw std::length_error( "Plaintext length must be divisible by " +
                                 std::to_string( blockBytesLen ) );
    }
}

void rijndael::EncryptBlock( const unsigned char in[], unsigned char out[],
                        unsigned char *roundKeys )
{
    unsigned char state[ 4 ][ Nb ];
    unsigned int i, j, round;

    for( i = 0; i < 4; i++ )
    {
        for( j = 0; j < Nb; j++ )
        {
            state[ i ][ j ] = in[ i + 4 * j ];
        }
    }

    AddRoundKey( state, roundKeys );

    for( round = 1; round <= Nr - 1; round++ )
    {
        SubBytes( state );
        ShiftRows( state );
        MixColumns( state );
        AddRoundKey( state, roundKeys + round * 4 * Nb );
    }

    SubBytes( state );
    ShiftRows( state );
    AddRoundKey( state, roundKeys + Nr * 4 * Nb );

    for( i = 0; i < 4; i++ )
    {
        for( j = 0; j < Nb; j++ )
        {
            out[ i + 4 * j ] = state[ i ][ j ];
        }
    }
}

void rijndael::DecryptBlock( const unsigned char in[], unsigned char out[],
                        unsigned char *roundKeys )
{
    unsigned char state[ 4 ][ Nb ];
    unsigned int i, j, round;

    for( i = 0; i < 4; i++ )
    {
        for( j = 0; j < Nb; j++ )
        {
            state[ i ][ j ] = in[ i + 4 * j ];
        }
    }

    AddRoundKey( state, roundKeys + Nr * 4 * Nb );

    for( round = Nr - 1; round >= 1; round-- )
    {
        InvSubBytes( state );
        InvShiftRows( state );
        AddRoundKey( state, roundKeys + round * 4 * Nb );
        InvMixColumns( state );
    }

    InvSubBytes( state );
    InvShiftRows( state );
    AddRoundKey( state, roundKeys );

    for( i = 0; i < 4; i++ )
    {
        for( j = 0; j < Nb; j++ )
        {
            out[ i + 4 * j ] = state[ i ][ j ];
        }
    }
}

void rijndael::SubBytes( unsigned char state[ 4 ][ Nb ] )
{
    unsigned int i, j;
    unsigned char t;
    for( i = 0; i < 4; i++ )
    {
        for( j = 0; j < Nb; j++ )
        {
            t = state[ i ][ j ];
            state[ i ][ j ] = sbox[ t / 16 ][ t % 16 ];
        }
    }
}

void rijndael::ShiftRow( unsigned char state[ 4 ][ Nb ], unsigned int i,
                    unsigned int n )  // shift row i on n write_positions
{
    unsigned char tmp[ Nb ];
    for( unsigned int j = 0; j < Nb; j++ )
    {
        tmp[ j ] = state[ i ][ ( j + n ) % Nb ];
    }
    memcpy( state[ i ], tmp, Nb * sizeof( unsigned char ) );
}

void rijndael::ShiftRows( unsigned char state[ 4 ][ Nb ] )
{
    ShiftRow( state, 1, 1 );
    ShiftRow( state, 2, 2 );
    ShiftRow( state, 3, 3 );
}

unsigned char rijndael::xtime( unsigned char b )  // multiply on x
{
    return ( b << 1 ) ^ ( ( ( b >> 7 ) & 1 ) * 0x1b );
}

void rijndael::MixColumns( unsigned char state[ 4 ][ Nb ] )
{
    unsigned char temp_state[ 4 ][ Nb ];

    for( size_t i = 0; i < 4; ++i )
    {
        memset( temp_state[ i ], 0, 4 );
    }

    for( size_t i = 0; i < 4; ++i )
    {
        for( size_t k = 0; k < 4; ++k )
        {
            for( size_t j = 0; j < 4; ++j )
            {
                if( CMDS[ i ][ k ] == 1 )
                    temp_state[ i ][ j ] ^= state[ k ][ j ];
                else
                    temp_state[ i ][ j ] ^= GF_MUL_TABLE[ CMDS[ i ][ k ] ][ state[ k ][ j ] ];
            }
        }
    }

    for( size_t i = 0; i < 4; ++i )
    {
        memcpy( state[ i ], temp_state[ i ], 4 );
    }
}

void rijndael::AddRoundKey( unsigned char state[ 4 ][ Nb ], unsigned char *key )
{
    unsigned int i, j;
    for( i = 0; i < 4; i++ )
    {
        for( j = 0; j < Nb; j++ )
        {
            state[ i ][ j ] = state[ i ][ j ] ^ key[ i + 4 * j ];
        }
    }
}

void rijndael::SubWord( unsigned char *a )
{
    int i;
    for( i = 0; i < 4; i++ )
    {
        a[ i ] = sbox[ a[ i ] / 16 ][ a[ i ] % 16 ];
    }
}

void rijndael::RotWord( unsigned char *a )
{
    unsigned char c = a[ 0 ];
    a[ 0 ] = a[ 1 ];
    a[ 1 ] = a[ 2 ];
    a[ 2 ] = a[ 3 ];
    a[ 3 ] = c;
}

void rijndael::XorWords( unsigned char *a, unsigned char *b, unsigned char *c )
{
    int i;
    for( i = 0; i < 4; i++ )
    {
        c[ i ] = a[ i ] ^ b[ i ];
    }
}

void rijndael::Rcon( unsigned char *a, unsigned int n )
{
    unsigned int i;
    unsigned char c = 1;
    for( i = 0; i < n - 1; i++ )
    {
        c = xtime( c );
    }

    a[ 0 ] = c;
    a[ 1 ] = a[ 2 ] = a[ 3 ] = 0;
}

void rijndael::KeyExpansion( const unsigned char key[], unsigned char w[] )
{
    unsigned char temp[ 4 ];
    unsigned char rcon[ 4 ];

    unsigned int i = 0;
    while( i < 4 * Nk )
    {
        w[ i ] = key[ i ];
        i++;
    }

    i = 4 * Nk;
    while( i < 4 * Nb * ( Nr + 1 ) )
    {
        temp[ 0 ] = w[ i - 4 + 0 ];
        temp[ 1 ] = w[ i - 4 + 1 ];
        temp[ 2 ] = w[ i - 4 + 2 ];
        temp[ 3 ] = w[ i - 4 + 3 ];

        if( i / 4 % Nk == 0 )
        {
            RotWord( temp );
            SubWord( temp );
            Rcon( rcon, i / ( Nk * 4 ) );
            XorWords( temp, rcon, temp );
        }
        else if( Nk > 6 && i / 4 % Nk == 4 )
        {
            SubWord( temp );
        }

        w[ i + 0 ] = w[ i - 4 * Nk ] ^ temp[ 0 ];
        w[ i + 1 ] = w[ i + 1 - 4 * Nk ] ^ temp[ 1 ];
        w[ i + 2 ] = w[ i + 2 - 4 * Nk ] ^ temp[ 2 ];
        w[ i + 3 ] = w[ i + 3 - 4 * Nk ] ^ temp[ 3 ];
        i += 4;
    }
}

void rijndael::InvSubBytes( unsigned char state[ 4 ][ Nb ] )
{
    unsigned int i, j;
    unsigned char t;
    for( i = 0; i < 4; i++ )
    {
        for( j = 0; j < Nb; j++ )
        {
            t = state[ i ][ j ];
            state[ i ][ j ] = inv_sbox[ t / 16 ][ t % 16 ];
        }
    }
}

void rijndael::InvMixColumns( unsigned char state[ 4 ][ Nb ] )
{
    unsigned char temp_state[ 4 ][ Nb ];

    for( size_t i = 0; i < 4; ++i )
    {
        memset( temp_state[ i ], 0, 4 );
    }

    for( size_t i = 0; i < 4; ++i )
    {
        for( size_t k = 0; k < 4; ++k )
        {
            for( size_t j = 0; j < 4; ++j )
            {
                temp_state[ i ][ j ] ^= GF_MUL_TABLE[ INV_CMDS[ i ][ k ] ][ state[ k ][ j ] ];
            }
        }
    }

    for( size_t i = 0; i < 4; ++i )
    {
        memcpy( state[ i ], temp_state[ i ], 4 );
    }
}

void rijndael::InvShiftRows( unsigned char state[ 4 ][ Nb ] )
{
    ShiftRow( state, 1, Nb - 1 );
    ShiftRow( state, 2, Nb - 2 );
    ShiftRow( state, 3, Nb - 3 );
}

void rijndael::XorBlocks( const unsigned char *a, const unsigned char *b,
                     unsigned char *c, unsigned int len )
{
    for( unsigned int i = 0; i < len; i++ )
    {
        c[ i ] = a[ i ] ^ b[ i ];
    }
}

std::vector<unsigned char> rijndael::ArrayToVector( unsigned char *a,
                                               unsigned int len )
{
    std::vector<unsigned char> v( a, a + len * sizeof( unsigned char ) );
    return v;
}

unsigned char *rijndael::VectorToArray( std::vector<unsigned char> &a )
{
    return a.data();
}

std::vector<unsigned char> rijndael::EncryptECB( std::vector<unsigned char> in,
                                            std::vector<unsigned char> key )
{
    unsigned char *out = EncryptECB( VectorToArray( in ), ( unsigned int )in.size(),
                                     VectorToArray( key ) );
    std::vector<unsigned char> v = ArrayToVector( out, in.size() );
    delete[] out;
    return v;
}

std::vector<unsigned char> rijndael::DecryptECB( std::vector<unsigned char> in,
                                            std::vector<unsigned char> key )
{
    unsigned char *out = DecryptECB( VectorToArray( in ), ( unsigned int )in.size(),
                                     VectorToArray( key ) );
    std::vector<unsigned char> v = ArrayToVector( out, ( unsigned int )in.size() );
    delete[] out;
    return v;
}