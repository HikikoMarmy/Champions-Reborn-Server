#include "RealmCharacterMetaKV.h"

#include "../Common/ByteStream.h"
#include "../Common/Utility.h"
#include "../logging.h"

RealmCharacterMetaData::RealmCharacterMetaData( const std::vector<uint8_t> &data )
{
	ByteBuffer stream( data );
	Deserialize( stream );
}

void RealmCharacterMetaData::Deserialize( ByteBuffer &stream )
{
	auto numberOfKeys = stream.read_u32();

	if( !Util::IsInRange< int32_t >( numberOfKeys, 0, 4 ) )
	{
		m_metaData.clear();
		return;
	}

	m_metaData.clear();
	m_metaData.reserve( numberOfKeys );

	for( auto i = 0; i < numberOfKeys; ++i )
	{
		std::wstring key = stream.read_utf16();
		m_metaData.emplace_back( key, L"" );
	}

	auto numberOfValues = stream.read_u32();

	for( auto &pair : m_metaData )
	{
		pair.second = stream.read_utf16();
	}
}

void RealmCharacterMetaData::Deserialize( const std::vector<uint8_t> &data )
{
	ByteBuffer stream( data );
	Deserialize( stream );
}

std::vector< uint8_t > RealmCharacterMetaData::Serialize() const
{
	ByteBuffer stream;

	// Number of keys
	stream.write_u32( static_cast< int32_t >( m_metaData.size() ) ); 

	for( const auto &pair : m_metaData )
	{
		stream.write_utf16( pair.first );
	}

	// Number of values
	stream.write_u32( static_cast< uint32_t >( m_metaData.size() ) );

	for( const auto &pair : m_metaData )
	{
		stream.write_utf16( pair.second );
	}

	return stream.m_buffer;
}
