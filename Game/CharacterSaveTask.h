#pragma once

#include <array>
#include <string>
#include <vector>
#include <unordered_map>

#include "../Common/Constant.h"
#include "../Common/ByteStream.h"
#include "RealmUser.h"
#include "RealmCharacterMetaKV.h"

enum class CharacterSaveType : uint8_t
{
    NEW_CHARACTER,
    SAVE_CHARACTER
};

class CharacterSaveTask {
public:
    CharacterSaveTask( CharacterSaveType Type, uint32_t characterId = 0 );
	~CharacterSaveTask();

	void SetMetaData( const CharacterSlotData &metaData );
	bool AppendData( const std::vector< uint8_t > &data );

	bool Validate();

	const std::vector< uint8_t >& GetData() const;

public:
    CharacterSaveType m_saveType;
	sptr_user m_ownerUser;
	sptr_user m_targetUser;

    uint32_t m_characterId;
    uint32_t m_writePosition;

    CharacterSlotData m_meta;
    std::vector< uint8_t > m_data;
};

using sptr_character_save_task = std::shared_ptr< CharacterSaveTask >;