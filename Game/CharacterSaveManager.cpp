/*
* CharacterSaveManager is responsible for managing character saves while online.
* Because Return To Arms can have any user in the party initiate a save for any
* other user in the party, we need to keep track of these tasks.
*
* Tasks keep track of the user that initiated the save (the owner),
* and the target user whose character is being saved.
*/

#include "CharacterSaveManager.h"

#include "RealmUser.h"
#include "RealmUserManager.h"
#include "../Database/Database.h"
#include "../../logging.h"

CharacterSaveManager::CharacterSaveManager()
{
	m_tasks.clear();
}

CharacterSaveManager::~CharacterSaveManager()
{
	m_tasks.clear();
}

bool CharacterSaveManager::BeginSaveTask(
	const sptr_user user,
	const uint32_t characterId,
	const CharacterSlotData &metaData,
	const CharacterSaveType saveType )
{
	return BeginSaveTask( user, user, characterId, metaData, saveType );
}

bool CharacterSaveManager::BeginSaveTask(
	const sptr_user m_owner,
	const sptr_user m_target,
	const uint32_t characterId,
	const CharacterSlotData &metaData,
	const CharacterSaveType saveType )
{
	if( !m_owner || !m_target )
	{
		Log::Error( "CreateSaveTask called with empty session IDs!" );
		return false;
	}

	const auto &sessionId = m_owner->m_sessionId;

	if( m_tasks.find( sessionId ) != m_tasks.end() )
	{
		m_tasks.erase( sessionId );
	}

	auto task = std::make_shared< CharacterSaveTask >( saveType, characterId );

	task->m_ownerUser = m_owner;
	task->m_targetUser = m_target;
	task->m_meta = metaData;
	m_tasks[ sessionId ] = task;

	return true;
}

void CharacterSaveManager::AppendSaveData( const std::wstring &sessionId, const std::vector<uint8_t> &data, bool endOfData )
{
	try
	{
		auto it = m_tasks.find( sessionId );
		if( it == m_tasks.end() )
		{
			Log::Error( "AddDataToSaveTask: No task found for session ID [{}].", sessionId );
			return;
		}

		auto &task = it->second;

		if( !task->AppendData( data ) )
		{
			Log::Error( "AddDataToSaveTask: Append failed for session ID [{}].", sessionId );
			return;
		}

		if( !endOfData )
		{
			return;
		}

		if( !task->Validate() )
		{
			Log::Error( "AddDataToSaveTask: Validation failed for session ID [{}].", sessionId );
			return;
		}

		if( !CommitSaveTask( sessionId ) )
		{
			Log::Error( "AddDataToSaveTask: Commit failed for session ID [{}].", sessionId );
			return;
		}

		Log::Debug( "AddDataToSaveTask: Final chunk committed for session ID [{}].", sessionId );
	}
	catch( const std::exception &e )
	{
		Log::Error( "AddDataToSaveTask: Exception for session ID [{}]: {}", sessionId, e.what() );
	}
}

bool CharacterSaveManager::CommitSaveTask( const std::wstring &sessionId )
{
	try
	{
		auto node = m_tasks.extract( sessionId );
		if( !node )
		{
			Log::Error( "CommitSaveTask: Task for session ID [{}] not found.", sessionId );
			return false;
		}

		const auto &saveTask = node.mapped();

		if( !saveTask->Validate() )
		{
			return false;
		}

		auto &targetUser = saveTask->m_targetUser;

		if( !targetUser )
		{
			Log::Error( "CommitSaveTask: Target user not found." );
			return false;
		}

		if( saveTask->m_saveType == CharacterSaveType::NEW_CHARACTER )
		{
			auto result = Database::Get().CreateNewCharacter( targetUser->m_accountId, saveTask->m_meta, saveTask->m_data );
			if( !result )
			{
				Log::Error( "CommitSaveTask: Failed to create new character for account ID: {}", targetUser->m_accountId );
				return false;
			}

			Log::Debug( "CommitSaveTask: New character created with ID {} for account ID: {}", result, targetUser->m_accountId );

			targetUser->m_characterId = result;
		}
		else
		{
			if( 0 == saveTask->m_characterId )
			{
				Log::Error( "CommitSaveTask: Invalid character ID for save task." );
				return false;
			}

			auto result = Database::Get().SaveCharacter( targetUser->m_accountId, targetUser->m_characterId, saveTask->m_meta, saveTask->m_data );
			if( !result )
			{
				Log::Error( "CommitSaveTask: Failed to save character for account ID: {}", targetUser->m_accountId );
				return false;
			}

			Log::Debug( "CommitSaveTask: Character saved for account ID: {}", targetUser->m_accountId );
		}
	}
	catch( const std::exception &e )
	{
		Log::Error( "CommitSaveTask: Exception occurred while committing task for session ID [{}]: {}", sessionId, e.what() );
		return false;
	}

	Log::Debug( "CommitSaveTask: Task for session ID [{}] committed successfully.", sessionId );
	return true;
}

void CharacterSaveManager::RemoveSaveTask( const std::wstring &sessionId )
{
	auto it = m_tasks.find( sessionId );
	if( it != m_tasks.end() )
	{
		m_tasks.erase( it );
		Log::Debug( "RemoveSaveTask: Task for session ID [{}] removed.", sessionId );
	}
	else
	{
		Log::Error( "RemoveSaveTask: Task for session ID [{}] not found.", sessionId );
	}
}

sptr_character_save_task CharacterSaveManager::FindSaveTask( const std::wstring &sessionId )
{
	auto it = m_tasks.find( sessionId );
	if( it != m_tasks.end() )
	{
		return it->second;
	}
	else
	{
		Log::Error( "FindSaveTask: Task for session ID [{}] not found.", sessionId );
		return nullptr;
	}
}
