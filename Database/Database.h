#pragma once

#include <memory>
#include <mutex>
#include <string>
#include <vector>
#include <map>
#include <sqlite3.h>

#include "../../Game/RealmCharacter.h"
#include "../../Game/RealmCharacterMetaKV.h"

enum class QueryID {
	CreateAccount,
	VerifyAccount,

	CreateSession,
	UpdateSession,
	DeleteSession,
	GetSession,

	CreateNewCharacter,
	SaveCharacter,
	SaveInventory,

	LoadCharacter,
	LoadAllCharacterMetaData,
};

class Database {
private:
	static inline std::unique_ptr<Database> m_instance;
	static inline std::mutex m_mutex;

	sqlite3* m_db = nullptr;
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

public:
	int64_t CreateNewAccount( const std::string &username,
						   const std::string &password,
						   const std::string &email_address,
						   const std::string &date_of_birth,
						   const std::string &chat_handle );

	int64_t VerifyAccount( const std::string &username, const std::string &password );

	bool CreateSession( const int64_t account_id, const std::wstring &session_id, const std::string &ip_address );
	bool UpdateSession( const std::wstring &session_id );
	bool DeleteSession( const std::wstring &session_id );
	int64_t GetSession(std::wstring& session_id, std::string& ip_address);

	int32_t CreateNewCharacter( const int64_t account_id,
							 const std::vector< uint8_t > &meta_data,
							 const std::vector< uint8_t > &character_blob );

	bool SaveCharacter( const int64_t account_id,
						const int32_t character_id,
						const std::vector< uint8_t > &meta_data,
						const std::vector< uint8_t > &character_blob );

	bool SaveInventory( const int64_t account_id,
						const int32_t character_id,
						const std::vector< uint8_t > &inventory_data );

	std::map< int32_t, RealmCharacterMetaData > LoadCharacterList( const int64_t account_id );

	sptr_realm_character LoadCharacterData( const int64_t account_id, const int32_t character_id );

private:
	void CreateTables();
	void PrepareStatements();
	void FinalizeStatements();
	void Execute( const char *sql );
};
