#pragma once

#include <string>
#include <vector>
#include <utility>

#include "../Common/ByteStream.h"

using CharacterMetaKV = std::pair<std::wstring, std::wstring>;

class RealmCharacterMetaData {
private:
    std::vector<CharacterMetaKV> m_metaData;

public:
    RealmCharacterMetaData() = default;
    explicit RealmCharacterMetaData( const std::vector<uint8_t> &data );

    void Deserialize( ByteBuffer &stream );
    void Deserialize( const std::vector<uint8_t> &data );

    std::vector<uint8_t> Serialize() const;

    const std::vector<CharacterMetaKV> &GetMetaData() const
    {
        return m_metaData;
    }

    void SetMetaData( const std::vector<CharacterMetaKV> &metaData )
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