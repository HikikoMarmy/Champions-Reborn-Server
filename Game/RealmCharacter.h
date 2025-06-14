#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "../Common/ByteStream.h"
#include "RealmCharacterMetaKV.h"

class RealmCharacter {
public:
	// Character metadata, such as name, class, etc.
	// This is used for displaying character information in the character selection screen.
	RealmCharacterMetaData m_metaData;		

	// Character data blob, such as stats, skills, etc.
	std::vector< uint8_t > m_characterData;

	// Inventory data blob
	std::vector< uint8_t > m_inventoryData;

public:
	RealmCharacter() = default;
	RealmCharacter( std::vector< uint8_t > &data );
	RealmCharacter(sptr_byte_stream stream);

	const std::vector< uint8_t > GetCharacterData() const;
	void SetCharacterData( const std::vector< uint8_t > &data );

	const std::vector< uint8_t > GetInventoryData() const;
	void SetInventoryData( const std::vector< uint8_t > &data );

	RealmCharacterMetaData GetMetaData() const;
	void SetMetaData( const std::vector< uint8_t > &data );

	const std::vector< uint8_t > Unpack();
	std::vector< uint8_t > Pack(const std::vector<uint8_t>& input);
};

using sptr_realm_character = std::shared_ptr< RealmCharacter >;