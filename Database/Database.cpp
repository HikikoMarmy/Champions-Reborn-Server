#include "Database.h"

#include "../../Crypto/PasswordHash.h"
#include "../../Game/RealmCharacter.h"
#include "../../Game/RealmCharacterMetaKV.h"
#include "../logging.h"

Database::Database()
{
	if( sqlite3_open( "./database/game.db", &m_db ) != SQLITE_OK )
	{
		throw std::runtime_error( "Failed to open DB: " + std::string( sqlite3_errmsg( m_db ) ) );
	}

	m_lastMaintenance = std::chrono::steady_clock::now();

	CreateTables();
	PrepareStatements();
}

Database::~Database()
{
	FinalizeStatements();
	if( m_db )
		sqlite3_close( m_db );
}

void Database::Process()
{
	auto now = std::chrono::steady_clock::now();
	if( now - m_lastMaintenance < std::chrono::hours( 1 ) )
		return;

	m_lastMaintenance = now;

	Log::Info( "Performing database maintenance..." );
}

void Database::CreateTables()
{
	Execute( "CREATE TABLE IF NOT EXISTS RealmUsers ("
			 "account_id INTEGER PRIMARY KEY AUTOINCREMENT,"
			 "username TEXT UNIQUE NOT NULL,"
			 "password TEXT NOT NULL,"
			 "email_address TEXT NOT NULL,"
			 "date_of_birth NOT NULL,"
			 "chat_handle TEXT NOT NULL)" );

	Execute( "CREATE TABLE IF NOT EXISTS RealmCharacters ("
			 "account_id INTEGER NOT NULL,"
			 "character_id INTEGER PRIMARY KEY AUTOINCREMENT,"
			 "meta_data BLOB NOT NULL,"
			 "character_data BLOB NOT NULL)" );

	Execute( "CREATE TABLE IF NOT EXISTS RealmSession ("
			 "account_id   INTEGER NOT NULL,"
			 "session_id   TEXT NOT NULL UNIQUE,"
			 "character_id INTEGER NOT NULL,"
			 "ip_address   TEXT NOT NULL,"
			 "expire_time  INTEGER NOT NULL,"
			 "PRIMARY KEY(session_id))" );

	Execute( "CREATE TABLE IF NOT EXISTS UserFriendList ("
			 "account_id    INTEGER NOT NULL,"
			 "friend_handle TEXT NOT NULL,"
			 "PRIMARY KEY(account_id, friend_handle))" );

	Execute( "CREATE TABLE IF NOT EXISTS UserIgnoredList ("
			 "account_id    INTEGER NOT NULL,"
			 "ignore_handle TEXT NOT NULL,"
			 "PRIMARY KEY(account_id, ignore_handle))" );
}

void Database::PrepareStatements()
{
	const std::vector<std::pair<QueryID, const char *>> queries = {
		{ QueryID::CreateAccount,
		"INSERT INTO RealmUsers ( username, password, email_address, date_of_birth, chat_handle ) VALUES ( ?, ?, ?, ?, ? );" },

		{ QueryID::VerifyAccount,
		"SELECT account_id, username, password, chat_handle FROM RealmUsers WHERE username = ?;" },

		{ QueryID::LoadAccount,
		"SELECT chat_handle FROM RealmUsers WHERE account_id = ?;" },

		{ QueryID::CreateNewCharacter,
		"INSERT INTO RealmCharacters ( account_id, meta_data, character_data ) VALUES ( ?, ?, ? );" },

		{ QueryID::SaveCharacter,
		"UPDATE RealmCharacters SET meta_data = ?, character_data = ? WHERE account_id = ? AND character_id = ?;" },

		{ QueryID::LoadCharacter,
		"SELECT character_id, meta_data, character_data FROM RealmCharacters WHERE account_id = ? AND character_id = ?;" },

		{ QueryID::LoadCharacterSlots,
		"SELECT character_id, meta_data FROM RealmCharacters WHERE account_id = ? ORDER BY character_id;" },

		{ QueryID::SaveFriend,
		"INSERT OR IGNORE INTO UserFriendList ( account_id, friend_handle ) VALUES ( ?, ? );" },

		{ QueryID::LoadFriendList,
		"SELECT friend_handle FROM UserFriendList WHERE account_id = ?;" },

		{ QueryID::SaveIgnore,
		"INSERT OR IGNORE INTO UserIgnoredList ( account_id, ignore_handle ) VALUES ( ?, ? );" },

		{ QueryID::LoadIgnoreList,
		"SELECT ignore_handle FROM UserIgnoredList WHERE account_id = ?;" }
	};

	try
	{
		for( const auto &[id, sql] : queries )
		{
			sqlite3_stmt *stmt = nullptr;
			if( sqlite3_prepare_v2( m_db, sql, -1, &stmt, nullptr ) != SQLITE_OK )
			{
				throw std::runtime_error( "Failed to prepare statement: " + std::string( sqlite3_errmsg( m_db ) ) );
			}
			m_statements[ id ] = stmt;
		}
	}
	catch( const std::exception &e )
	{
		FinalizeStatements();
		Log::Error( "Database error: {}", std::string( e.what() ) );
	}
}

void Database::FinalizeStatements()
{
	if( m_db == nullptr )
		return;

	for( const auto &[id, stmt] : m_statements )
	{
		if( stmt ) sqlite3_finalize( stmt );
	}
	m_statements.clear();
}

void Database::Execute( const char *sql )
{
	try
	{
		char *errMsg = nullptr;
		if( sqlite3_exec( m_db, sql, nullptr, nullptr, &errMsg ) != SQLITE_OK )
		{
			std::string error = "SQL execution failed: " + std::string( errMsg );
			sqlite3_free( errMsg );
			throw std::runtime_error( error );
		}
	}
	catch( const std::exception &e )
	{
		Log::Error( "Database error: {}", std::string( e.what() ) );
	}
}

int64_t Database::CreateNewAccount( const std::string &username,
									const std::string &password,
									const std::string &email_address,
									const std::string &date_of_birth,
									const std::string &chat_handle )
{
	try
	{
		auto hashedPassword = HashPassword( password, 1000, 32 );
		auto stmt = m_statements[ QueryID::CreateAccount ];

		SQLiteTransaction tx( m_db );
		{
			sqlite3_reset( stmt );
			sqlite3_clear_bindings( stmt );

			sqlite3_bind_text( stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT );
			sqlite3_bind_text( stmt, 2, hashedPassword.c_str(), -1, SQLITE_TRANSIENT );
			sqlite3_bind_text( stmt, 3, email_address.c_str(), -1, SQLITE_TRANSIENT );
			sqlite3_bind_text( stmt, 4, date_of_birth.c_str(), -1, SQLITE_TRANSIENT );
			sqlite3_bind_text( stmt, 5, chat_handle.c_str(), -1, SQLITE_TRANSIENT );

			if( sqlite3_step( stmt ) != SQLITE_DONE )
			{
				throw std::runtime_error( "Insert failed: " + std::string( sqlite3_errmsg( m_db ) ) );
			}
		}
		tx.commit();

		return sqlite3_last_insert_rowid( m_db );
	}
	catch( const std::exception &e )
	{
		throw std::runtime_error( "Database error: " + std::string( e.what() ) );
	}

	return 0;
}

std::tuple< bool, int64_t, std::wstring >
Database::VerifyAccount( const std::wstring &username, const std::wstring &password )
{
	try
	{
		auto stmt = m_statements[ QueryID::VerifyAccount ];
		auto username_utf8 = Util::WideToUTF8( username );
		auto password_utf8 = Util::WideToUTF8( password );

		sqlite3_reset( stmt );
		sqlite3_clear_bindings( stmt );

		sqlite3_bind_text( stmt, 1, username_utf8.c_str(), -1, SQLITE_TRANSIENT );

		// Execute the statement
		if( sqlite3_step( stmt ) == SQLITE_ROW )
		{
			int64_t accountId = sqlite3_column_int64( stmt, 0 );
			const char *dbUsername = reinterpret_cast< const char * >( sqlite3_column_text( stmt, 1 ) );
			const char *dbPassword = reinterpret_cast< const char * >( sqlite3_column_text( stmt, 2 ) );
			const char *dbChatHandle = reinterpret_cast< const char * >( sqlite3_column_text( stmt, 3 ) );

			if( username_utf8 == dbUsername && VerifyPassword( password_utf8, dbPassword ) )
			{
				Log::Debug( "Account verified: {} (ID: {})", username, accountId );
				return std::make_tuple( true, accountId, Util::UTF8ToWide( dbChatHandle ) );
			}
			else
			{
				Log::Error( "Invalid credentials for account ID: {}", accountId );
				return std::make_tuple( false, -1, L"" );
			}
		}
		else if( sqlite3_step( stmt ) == SQLITE_DONE )
		{
			return std::make_tuple( false, -1, L"" ); // No matching account found
		}
		else
		{
			throw std::runtime_error( "Query failed: " + std::string( sqlite3_errmsg( m_db ) ) );
		}
	}
	catch( const std::exception &e )
	{
		Log::Error( "Database error: {}", std::string( e.what() ) );
	}

	return std::make_tuple( false, -1, L"" );
}

uint32_t Database::CreateNewCharacter( const int64_t account_id, const CharacterSlotData meta, const std::vector< uint8_t > &blob )
{
	if( account_id <= 0 || meta.empty() || blob.empty() )
	{
		Log::Error( "Invalid parameters for CreateNewCharacter" );
		return 0;
	}

	try
	{
		auto stmt = m_statements[ QueryID::CreateNewCharacter ];
		const auto meta_data = meta.Serialize();

		SQLiteTransaction tx( m_db );
		{
			sqlite3_reset( stmt );
			sqlite3_clear_bindings( stmt );

			sqlite3_bind_int64( stmt, 1, account_id );
			sqlite3_bind_blob( stmt, 2, meta_data.data(), static_cast< int >( meta_data.size() ), SQLITE_STATIC );
			sqlite3_bind_blob( stmt, 3, blob.data(), static_cast< int >( blob.size() ), SQLITE_STATIC );

			int rc = sqlite3_step( stmt );
			if( rc != SQLITE_DONE )
			{
				Log::Error( "SQLite insert failed: {}", sqlite3_errmsg( m_db ) );
				return 0;
			}
		}
		tx.commit();

		uint32_t character_id = static_cast< uint32_t >( sqlite3_last_insert_rowid( m_db ) );

		return character_id;
	}
	catch( const std::exception &e )
	{
		Log::Error( "Database error: {}", std::string( e.what() ) );
	}

	return 0;
}

bool Database::SaveCharacter( const int64_t account_id, const int32_t character_id, const CharacterSlotData meta, const std::vector< uint8_t > &blob )
{
	if( account_id <= 0 || character_id <= 0 || meta.empty() || blob.empty() )
	{
		Log::Error( "Invalid parameters for SaveCharacter" );
		return false;
	}

	try
	{
		auto stmt = m_statements[ QueryID::SaveCharacter ];
		const auto meta_data = meta.Serialize();

		SQLiteTransaction tx( m_db );
		{
			sqlite3_reset( stmt );
			sqlite3_clear_bindings( stmt );

			sqlite3_bind_blob( stmt, 1, meta_data.data(), static_cast< int >( meta_data.size() ), SQLITE_STATIC );
			sqlite3_bind_blob( stmt, 2, blob.data(), static_cast< int >( blob.size() ), SQLITE_STATIC );
			sqlite3_bind_int64( stmt, 3, account_id );
			sqlite3_bind_int( stmt, 4, character_id );

			if( sqlite3_step( stmt ) != SQLITE_DONE )
			{
				throw std::runtime_error( "Update failed: " + std::string( sqlite3_errmsg( m_db ) ) );
			}
		}
		tx.commit();

		return true;
	}
	catch( const std::exception &e )
	{
		Log::Error( "Database error: {}", std::string( e.what() ) );
	}

	return false;
}

std::map< uint32_t, CharacterSlotData > Database::LoadCharacterSlots( const int64_t account_id )
{
	std::map< uint32_t, CharacterSlotData > result;

	try
	{
		auto stmt = m_statements[ QueryID::LoadCharacterSlots ];
		sqlite3_reset( stmt );
		sqlite3_clear_bindings( stmt );

		sqlite3_bind_int64( stmt, 1, account_id );

		while( sqlite3_step( stmt ) == SQLITE_ROW )
		{
			const auto character_id = sqlite3_column_int( stmt, 0 );

			const auto *slot_data_blob = static_cast< const uint8_t * >( sqlite3_column_blob( stmt, 1 ) );
			const auto slot_data_size = sqlite3_column_bytes( stmt, 1 );

			if( slot_data_blob == nullptr || slot_data_size == 0 )
			{
				Log::Error( "Character ID {} has no metadata", character_id );
				continue;
			}

			CharacterSlotData slot_data( std::vector< uint8_t >( slot_data_blob, slot_data_blob + slot_data_size ) );

			result.insert( { character_id, slot_data } );
		}
	}
	catch( const std::exception &e )
	{
		Log::Error( "Database error: {}", std::string( e.what() ) );
		result.clear();
	}

	return result;
}

sptr_realm_character Database::LoadCharacterData( const int64_t account_id, const int32_t character_id )
{
	try
	{
		auto data = std::make_shared< RealmCharacter >();
		auto stmt = m_statements[ QueryID::LoadCharacter ];

		sqlite3_reset( stmt );
		sqlite3_clear_bindings( stmt );

		sqlite3_bind_int64( stmt, 1, account_id );
		sqlite3_bind_int( stmt, 2, character_id );

		if( sqlite3_step( stmt ) == SQLITE_ROW )
		{
			const auto character_id = sqlite3_column_int( stmt, 0 );

			const auto *meta_data_blob = static_cast< const uint8_t * >( sqlite3_column_blob( stmt, 1 ) );
			const auto meta_data_size = sqlite3_column_bytes( stmt, 1 );

			if( meta_data_blob && meta_data_size > 0 )
			{
				data->Deserialize( std::vector< uint8_t >( meta_data_blob, meta_data_blob + meta_data_size ) );
			}

			const auto *character_blob = static_cast< const uint8_t * >( sqlite3_column_blob( stmt, 2 ) );
			const auto character_size = sqlite3_column_bytes( stmt, 2 );

			if( character_blob && character_size > 0 )
			{
				data->m_characterId = character_id;
				data->m_data = std::vector< uint8_t >( character_blob, character_blob + character_size );
			}

			Log::Debug( "Character data loaded for account ID: {} and character ID: {}", account_id, character_id );
			return data;
		}
		else
		{
			Log::Error( "No character data found for account ID: {} and character ID: {}", account_id, character_id );
		}
	}
	catch( const std::exception &e )
	{
		Log::Error( "Database error: {}", std::string( e.what() ) );
	}

	return nullptr;
}

bool Database::SaveFriend( const int64_t account_id, const std::wstring &friend_handle )
{
	if( account_id <= 0 || friend_handle.empty() )
	{
		Log::Error( "Invalid parameters for SaveFriend" );
		return false;
	}

	try
	{
		auto stmt = m_statements[ QueryID::SaveFriend ];
		auto friendHandle = Util::WideToUTF8( friend_handle );
		SQLiteTransaction tx( m_db );
		{
			sqlite3_reset( stmt );
			sqlite3_clear_bindings( stmt );
			sqlite3_bind_int64( stmt, 1, account_id );
			sqlite3_bind_text( stmt, 2, friendHandle.c_str(), -1, SQLITE_TRANSIENT );

			if( sqlite3_step( stmt ) != SQLITE_DONE )
			{
				Log::Error( "SQLite insert failed: {}", sqlite3_errmsg( m_db ) );
				return false;
			}
		}
		tx.commit();
		return true;
	}
	catch( const std::exception &e )
	{
		Log::Error( "Database error: {}", std::string( e.what() ) );
	}
	return false;
}

std::vector<std::wstring> Database::LoadFriends( const int64_t account_id )
{
	std::vector<std::wstring> friend_list;

	try
	{
		auto stmt = m_statements[ QueryID::LoadFriendList ];
		sqlite3_reset( stmt );
		sqlite3_clear_bindings( stmt );
		sqlite3_bind_int64( stmt, 1, account_id );

		while( sqlite3_step( stmt ) == SQLITE_ROW )
		{
			const char *friendHandle = reinterpret_cast< const char * >( sqlite3_column_text( stmt, 0 ) );
			if( friendHandle )
			{
				friend_list.push_back( Util::UTF8ToWide( friendHandle ) );
			}
		}
	}
	catch( const std::exception &e )
	{
		Log::Error( "Database error: {}", std::string( e.what() ) );
	}

	return friend_list;
}

bool Database::SaveIgnore( const int64_t account_id, const std::wstring &ignore_handle )
{
	if( account_id <= 0 || ignore_handle.empty() )
	{
		Log::Error( "Invalid parameters for SaveIgnore" );
		return false;
	}

	try
	{
		auto stmt = m_statements[ QueryID::SaveIgnore ];
		auto friendHandle = Util::WideToUTF8( ignore_handle );
		SQLiteTransaction tx( m_db );
		{
			sqlite3_reset( stmt );
			sqlite3_clear_bindings( stmt );
			sqlite3_bind_int64( stmt, 1, account_id );
			sqlite3_bind_text( stmt, 2, friendHandle.c_str(), -1, SQLITE_TRANSIENT );

			if( sqlite3_step( stmt ) != SQLITE_DONE )
			{
				Log::Error( "SQLite insert failed: {}", sqlite3_errmsg( m_db ) );
				return false;
			}
		}

		tx.commit();
		return true;
	}
	catch( const std::exception &e )
	{
		Log::Error( "Database error: {}", std::string( e.what() ) );
	}
	return false;
}

std::vector<std::wstring> Database::LoadIgnores( const int64_t account_id )
{
	std::vector<std::wstring> ignore_list;

	try
	{
		auto stmt = m_statements[ QueryID::LoadIgnoreList ];
		sqlite3_reset( stmt );
		sqlite3_clear_bindings( stmt );
		sqlite3_bind_int64( stmt, 1, account_id );

		while( sqlite3_step( stmt ) == SQLITE_ROW )
		{
			const char *ignoreHandle = reinterpret_cast< const char * >( sqlite3_column_text( stmt, 0 ) );
			if( ignoreHandle )
			{
				ignore_list.push_back( Util::UTF8ToWide( ignoreHandle ) );
			}
		}
	}
	catch( const std::exception &e )
	{
		Log::Error( "Database error: {}", std::string( e.what() ) );
	}

	return ignore_list;
}
