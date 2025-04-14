#include "rijndael.h"

rijndael::rijndael()
{
	this->Nk = 8;
	this->Nr = 14;
}

uint8_t *rijndael::EncryptECB( const uint8_t in[], uint32_t inLen,
							   const uint8_t key[] )
{
	CheckLength( inLen );
	uint8_t *out = new uint8_t[ inLen ];
	uint8_t *roundKeys = new uint8_t[ 4 * Nb * ( Nr + 1 ) ];
	KeyExpansion( key, roundKeys );
	for( uint32_t i = 0; i < inLen; i += blockBytesLen )
	{
		EncryptBlock( in + i, out + i, roundKeys );
	}

	delete[] roundKeys;

	return out;
}

uint8_t *rijndael::DecryptECB( const uint8_t in[], uint32_t inLen,
							   const uint8_t key[] )
{
	CheckLength( inLen );
	uint8_t *out = new uint8_t[ inLen ];
	uint8_t *roundKeys = new uint8_t[ 4 * Nb * ( Nr + 1 ) ];
	KeyExpansion( key, roundKeys );

	for( uint32_t i = 0; i < inLen; i += blockBytesLen )
	{
		DecryptBlock( in + i, out + i, roundKeys );
	}

	delete[] roundKeys;

	return out;
}

void rijndael::CheckLength( uint32_t len )
{
	if( len % blockBytesLen != 0 )
	{
		throw std::length_error( "Plaintext length must be divisible by " +
								 std::to_string( blockBytesLen ) );
	}
}

void rijndael::EncryptBlock( const uint8_t in[], uint8_t out[],
							 uint8_t *roundKeys )
{
	uint8_t state[ 4 ][ Nb ];
	uint32_t i, j, round;

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

void rijndael::DecryptBlock( const uint8_t in[], uint8_t out[],
							 uint8_t *roundKeys )
{
	uint8_t state[ 4 ][ Nb ];
	uint32_t i, j, round;

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

void rijndael::SubBytes( uint8_t state[ 4 ][ Nb ] )
{
	uint32_t i, j;
	uint8_t t;
	for( i = 0; i < 4; i++ )
	{
		for( j = 0; j < Nb; j++ )
		{
			t = state[ i ][ j ];
			state[ i ][ j ] = sbox[ t / 16 ][ t % 16 ];
		}
	}
}

void rijndael::ShiftRow( uint8_t state[ 4 ][ Nb ], uint32_t i,
						 uint32_t n )  // shift row i on n write_positions
{
	uint8_t tmp[ Nb ];
	for( uint32_t j = 0; j < Nb; j++ )
	{
		tmp[ j ] = state[ i ][ ( j + n ) % Nb ];
	}
	memcpy( state[ i ], tmp, Nb * sizeof( uint8_t ) );
}

void rijndael::ShiftRows( uint8_t state[ 4 ][ Nb ] )
{
	ShiftRow( state, 1, 1 );
	ShiftRow( state, 2, 2 );
	ShiftRow( state, 3, 3 );
}

uint8_t rijndael::xtime( uint8_t b )  // multiply on x
{
	return ( b << 1 ) ^ ( ( ( b >> 7 ) & 1 ) * 0x1b );
}

void rijndael::MixColumns( uint8_t state[ 4 ][ Nb ] )
{
	uint8_t temp_state[ 4 ][ Nb ];

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

void rijndael::AddRoundKey( uint8_t state[ 4 ][ Nb ], uint8_t *key )
{
	uint32_t i, j;
	for( i = 0; i < 4; i++ )
	{
		for( j = 0; j < Nb; j++ )
		{
			state[ i ][ j ] = state[ i ][ j ] ^ key[ i + 4 * j ];
		}
	}
}

void rijndael::SubWord( uint8_t *a )
{
	int i;
	for( i = 0; i < 4; i++ )
	{
		a[ i ] = sbox[ a[ i ] / 16 ][ a[ i ] % 16 ];
	}
}

void rijndael::RotWord( uint8_t *a )
{
	uint8_t c = a[ 0 ];
	a[ 0 ] = a[ 1 ];
	a[ 1 ] = a[ 2 ];
	a[ 2 ] = a[ 3 ];
	a[ 3 ] = c;
}

void rijndael::XorWords( uint8_t *a, uint8_t *b, uint8_t *c )
{
	int i;
	for( i = 0; i < 4; i++ )
	{
		c[ i ] = a[ i ] ^ b[ i ];
	}
}

void rijndael::Rcon( uint8_t *a, uint32_t n )
{
	uint32_t i;
	uint8_t c = 1;
	for( i = 0; i < n - 1; i++ )
	{
		c = xtime( c );
	}

	a[ 0 ] = c;
	a[ 1 ] = a[ 2 ] = a[ 3 ] = 0;
}

void rijndael::KeyExpansion( const uint8_t key[], uint8_t w[] )
{
	uint8_t temp[ 4 ];
	uint8_t rcon[ 4 ];

	uint32_t i = 0;
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

void rijndael::InvSubBytes( uint8_t state[ 4 ][ Nb ] )
{
	uint32_t i, j;
	uint8_t t;
	for( i = 0; i < 4; i++ )
	{
		for( j = 0; j < Nb; j++ )
		{
			t = state[ i ][ j ];
			state[ i ][ j ] = inv_sbox[ t / 16 ][ t % 16 ];
		}
	}
}

void rijndael::InvMixColumns( uint8_t state[ 4 ][ Nb ] )
{
	uint8_t temp_state[ 4 ][ Nb ];

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

void rijndael::InvShiftRows( uint8_t state[ 4 ][ Nb ] )
{
	ShiftRow( state, 1, Nb - 1 );
	ShiftRow( state, 2, Nb - 2 );
	ShiftRow( state, 3, Nb - 3 );
}

void rijndael::XorBlocks( const uint8_t *a, const uint8_t *b,
						  uint8_t *c, uint32_t len )
{
	for( uint32_t i = 0; i < len; i++ )
	{
		c[ i ] = a[ i ] ^ b[ i ];
	}
}

std::vector<uint8_t> rijndael::ArrayToVector( uint8_t *a,
											  uint32_t len )
{
	std::vector<uint8_t> v( a, a + len * sizeof( uint8_t ) );
	return v;
}

uint8_t *rijndael::VectorToArray( std::vector<uint8_t> &a )
{
	return a.data();
}

std::vector<uint8_t> rijndael::EncryptECB( std::vector<uint8_t> in,
										   std::vector<uint8_t> key )
{
	uint8_t *out = EncryptECB( VectorToArray( in ), ( uint32_t )in.size(),
							   VectorToArray( key ) );
	std::vector<uint8_t> v = ArrayToVector( out, static_cast< uint32_t >( in.size() ) );
	delete[] out;
	return v;
}

std::vector<uint8_t> rijndael::DecryptECB( std::vector<uint8_t> in,
										   std::vector<uint8_t> key )
{
	uint8_t *out = DecryptECB( VectorToArray( in ), ( uint32_t )in.size(),
							   VectorToArray( key ) );
	std::vector<uint8_t> v = ArrayToVector( out, ( uint32_t )in.size() );
	delete[] out;
	return v;
}