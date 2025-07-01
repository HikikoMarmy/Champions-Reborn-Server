#include "RealmCharacterMetaKV.h"

#include "../Common/ByteBufferReader.hpp"
#include "../Common/ByteStream.h"
#include "../Common/Utility.h"
#include "../logging.h"

CharacterSlotData::CharacterSlotData(const std::vector<uint8_t>& data)
{
	if( !data.empty() )
	{
		Deserialize(data);
	}
	else
	{
		m_metaData.clear();
	}
}

void CharacterSlotData::Deserialize(const std::vector< uint8_t >& data)
{
	ByteBuffer reader(data);

	auto numberOfKeys = reader.read_i32();
	if( !Util::IsInRange( numberOfKeys, 0, 4 ) )
	{
		m_metaData.clear();
		return;
	}

	m_metaData.clear();
	m_metaData.reserve( numberOfKeys );

	for( auto i = 0; i < numberOfKeys; ++i )
	{
		std::wstring key = reader.read_utf16();
		m_metaData.emplace_back( key, L"" );
	}

	auto numberOfValues = reader.read_i32();
	for( auto &pair : m_metaData )
	{
		pair.second = reader.read_utf16();
	}
}

void CharacterSlotData::Deserialize(const sptr_byte_stream stream)
{
	auto numberOfKeys = stream->read_i32();
	if (!Util::IsInRange(numberOfKeys, 0, 4))
	{
		m_metaData.clear();
		return;
	}

	m_metaData.clear();
	m_metaData.reserve(numberOfKeys);

	for (auto i = 0; i < numberOfKeys; ++i)
	{
		std::wstring key = stream->read_utf16();
		m_metaData.emplace_back(key, L"");
	}

	auto numberOfValues = stream->read_i32();
	for (auto& pair : m_metaData)
	{
		pair.second = stream->read_utf16();
	}
}

std::vector< uint8_t > CharacterSlotData::Serialize() const
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
