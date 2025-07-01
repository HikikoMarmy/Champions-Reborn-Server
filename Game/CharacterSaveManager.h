#pragma once

#include <string>
#include <memory>
#include <unordered_map>

#include "CharacterSaveTask.h"

class CharacterSaveManager {
public:
	CharacterSaveManager();
	~CharacterSaveManager();
	CharacterSaveManager( const CharacterSaveManager & ) = delete;
	CharacterSaveManager &operator=( const CharacterSaveManager & ) = delete;

	static CharacterSaveManager &Get()
	{
		static CharacterSaveManager instance;
		return instance;
	}

	bool BeginSaveTask(
		const sptr_user user,
		const uint32_t characterId,
		const CharacterSlotData &metaData,
		const CharacterSaveType saveType );

	bool BeginSaveTask(
		const sptr_user m_owner,
		const sptr_user m_target,
		const uint32_t characterId,
		const CharacterSlotData &metaData,
		const CharacterSaveType saveType );

	void AppendSaveData( const std::wstring &sessionId, const std::vector<uint8_t> &data, bool endOfData );
	bool CommitSaveTask( const std::wstring &sessionId );
	void RemoveSaveTask( const std::wstring &sessionId );

	sptr_character_save_task FindSaveTask( const std::wstring &sessionId );

public:
	std::unordered_map< std::wstring, sptr_character_save_task > m_tasks;
};