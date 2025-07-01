#pragma once

#include <string>
#include <vector>
#include <utility>

#include "../Common/ByteStream.h"

using CharacterAttributeKV = std::pair<std::wstring, std::wstring>;

class CharacterSlotData {
private:
	std::vector<CharacterAttributeKV> m_metaData;

public:
	CharacterSlotData() = default;
	CharacterSlotData( const std::vector< uint8_t > &data );

	void Deserialize( const std::vector< uint8_t > &data );
	void Deserialize( const sptr_byte_stream stream );

	std::vector<uint8_t> Serialize() const;

	bool empty() const
	{
		return m_metaData.empty();
	}

	const std::vector<CharacterAttributeKV> &GetMetaData() const
	{
		return m_metaData;
	}

	void SetMetaData( const std::vector<CharacterAttributeKV> &metaData )
	{
		m_metaData = metaData;
	}

	std::wstring GetValue( std::wstring key )
	{
		for( const auto &kv : m_metaData )
		{
			if( kv.first == key )
			{
				return kv.second;
			}
		}
		return L"";
	}
};