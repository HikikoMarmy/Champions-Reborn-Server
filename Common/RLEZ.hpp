#pragma once

#include <vector>
#include <stdexcept>
#include <cstdint>

namespace RLEZ
{
	inline std::vector<uint8_t> Decompress( const std::vector<uint8_t> &input )
	{
		std::vector< uint8_t > output;
		size_t read = 0;

		while( read < input.size() )
		{
			uint8_t byte = input[ read++ ];
			output.push_back( byte );

			if( byte == 0x00 )
			{
				if( read >= input.size() )
				{
					break;
				}

				uint8_t count = input[ read++ ];
				output.insert( output.end(), count, 0x00 );
			}
		}

		return output;
	}

	inline std::vector<uint8_t> Compress( const std::vector<uint8_t> &input )
	{
		std::vector< uint8_t > output;
		size_t i = 0;

		while( i < input.size() )
		{
			if( input[ i ] != 0x00 )
			{
				output.push_back( input[ i ] );
				++i;
			}
			else
			{
				size_t zeroCount = 0;
				i++;

				// Count all proceeding 00's
				while( i < input.size() && input[ i ] == 0x00 && zeroCount < 255 )
				{
					++i;
					++zeroCount;
				}

				output.push_back( 0x00 );
				output.push_back( static_cast< uint8_t >( zeroCount ) );
			}
		}

		return output;
	}
}
