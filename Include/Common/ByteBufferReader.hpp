#pragma once
#include <vector>
#include <cstdint>
#include <stdexcept>
#include <cstring>

class ByteBufferReader {
public:
    ByteBufferReader( const uint8_t *data, size_t size )
        : buffer( data ), bufferSize( size ), offset( 0 )
    {
    }

    explicit ByteBufferReader( const std::vector<uint8_t> &vec )
        : ByteBufferReader( vec.data(), vec.size() )
    {
    }

    template<typename T>
    T read()
    {
        if( offset + sizeof( T ) > bufferSize )
            throw std::out_of_range( "BufferReader: read out of bounds" );

        T value;
        std::memcpy( &value, buffer + offset, sizeof( T ) );
        offset += sizeof( T );
        return value;
    }

    void readBytes( void *dest, size_t size )
    {
        if( offset + size > bufferSize )
            throw std::out_of_range( "BufferReader: readBytes out of bounds" );

        std::memcpy( dest, buffer + offset, size );
        offset += size;
    }

	std::string readString( size_t length )
	{
		if( offset + length > bufferSize )
			throw std::out_of_range( "BufferReader: readString out of bounds" );
		std::string str( reinterpret_cast< const char * >( buffer + offset ), length );
		offset += length;
		return str;
	}


    template<typename T, size_t N>
    std::array<T, N> readArray()
    {
        std::array<T, N> arr;
        for( size_t i = 0; i < N; ++i )
            arr[ i ] = read<T>();
        return arr;
    }

    void skip( size_t count )
    {
        if( offset + count > bufferSize )
            throw std::out_of_range( "BufferReader: skip out of bounds" );
        offset += count;
    }

    template<typename T>
    T peek() const
    {
        if( offset + sizeof( T ) > bufferSize )
            throw std::out_of_range( "BufferReader: peek out of bounds" );
        T value;
        std::memcpy( &value, buffer + offset, sizeof( T ) );
        return value;
    }

    bool eof() const
    {
        return offset >= bufferSize;
    }

    size_t tell() const
    {
        return offset;
    }

    size_t remaining() const
    {
        return bufferSize - offset;
    }

private:
    const uint8_t *buffer;
    size_t bufferSize;
    size_t offset;
};
