#pragma once

#include <memory>
#include <mutex>
#include <string>
#include <tuple>
#include <chrono>
#include <map>

#include <sqlite3.h>
#include "Transaction.h"
#include "../../Game/RealmCharacter.h"
#include "../../Game/RealmCharacterMetaKV.h"

enum class QueryID {
	CreateAccount,
	VerifyAccount,
	LoadAccount,

	LoadCharacterSlots,
	CreateNewCharacter,
	SaveCharacter,
	LoadCharacter,

	SaveFriend,
	LoadFriendList,

	SaveIgnore,
	LoadIgnoreList,
};

class Database {
private:
	static inline std::unique_ptr<Database> m_instance;
	static inline std::mutex m_mutex;
	static inline std::chrono::time_point< std::chrono::steady_clock > m_lastMaintenance;

	sqlite3 *m_db = nullptr;
	std::unordered_map< QueryID, sqlite3_stmt * > m_statements;

public:
	static Database &Get()
	{
		std::lock_guard<std::mutex> lock( m_mutex );
		if( !m_instance )
			m_instance.reset( new Database() );
		return *m_instance;
	}

	Database( const Database & ) = delete;
	Database &operator=( const Database & ) = delete;

	Database();
	~Database();

	void Process();

public:
	int64_t CreateNewAccount( const std::string &username,
							  const std::string &password,
							  const std::string &email_address,
							  const std::string &date_of_birth,
							  const std::string &chat_handle );

	std::tuple< bool, int64_t, std::wstring > VerifyAccount( const std::wstring &username, const std::wstring &password );

	uint32_t CreateNewCharacter( const int64_t account_id,
								 const CharacterSlotData meta,
								 const std::vector< uint8_t > &blob );

	bool SaveCharacter( const int64_t account_id,
						const int32_t character_id,
						const CharacterSlotData meta,
						const std::vector< uint8_t > &blob );

	std::map< uint32_t, CharacterSlotData > LoadCharacterSlots( const int64_t account_id );

	sptr_realm_character LoadCharacterData( const int64_t account_id, const int32_t character_id );

	bool SaveFriend( const int64_t account_id, const std::wstring &friend_handle );
	std::vector< std::wstring > LoadFriends( const int64_t account_id );

	bool SaveIgnore( const int64_t account_id, const std::wstring &ignore_handle );
	std::vector< std::wstring > LoadIgnores( const int64_t account_id );

private:
	void CreateTables();
	void PrepareStatements();
	void FinalizeStatements();
	void Execute( const char *sql );
};
