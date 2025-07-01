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

	DeleteOldSessions();
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
}

void Database::PrepareStatements()
{
	const std::vector<std::pair<QueryID, const char *>> queries = {
		{ QueryID::CreateAccount,
		"INSERT INTO RealmUsers ( username, password, email_address, date_of_birth, chat_handle ) VALUES ( ?, ?, ?, ?, ? );" },

		{ QueryID::VerifyAccount,
		"SELECT account_id, username, password FROM RealmUsers WHERE username = ?;" },

		{ QueryID::LoadAccount,
		"SELECT chat_handle FROM RealmUsers WHERE account_id = ?;" },

		{ QueryID::CreateSession,
		"INSERT INTO RealmSession ( account_id, session_id, character_id, ip_address, expire_time ) VALUES ( ?, ?, ?, ?, ? );" },

		{ QueryID::UpdateSession,
		"UPDATE RealmSession SET expire_time = ?, character_id = ? WHERE session_id = ?;" },

		{ QueryID::DeleteSession,
		"DELETE FROM RealmSession WHERE session_id = ?;" },

		{ QueryID::GetSession,
		"SELECT account_id, character_id, expire_time FROM RealmSession WHERE session_id = ? AND ip_address = ?;" },

		{ QueryID::DeleteOldSessions,
		"DELETE FROM RealmSession WHERE expire_time < ?" },

		{ QueryID::CreateNewCharacter,
		"INSERT INTO RealmCharacters ( account_id, meta_data, character_data ) VALUES ( ?, ?, ? );" },

		{ QueryID::SaveCharacter,
		"UPDATE RealmCharacters SET meta_data = ?, character_data = ? WHERE account_id = ? AND character_id = ?;" },

		{ QueryID::LoadCharacter,
		"SELECT character_id, meta_data, character_data FROM RealmCharacters WHERE account_id = ? AND character_id = ?;" },

		{ QueryID::LoadCharacterSlots,
		"SELECT character_id, meta_data FROM RealmCharacters WHERE account_id = ? ORDER BY character_id;" }
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

void Database::DeleteOldSessions()
{
	try
	{
		auto stmt = m_statements[ QueryID::DeleteOldSessions ];

		sqlite3_reset( stmt );
		sqlite3_clear_bindings( stmt );

		int64_t currentTime = time( nullptr );
		sqlite3_bind_int64( stmt, 1, currentTime );

		if( sqlite3_step( stmt ) != SQLITE_DONE )
		{
			throw std::runtime_error( "Delete old sessions failed: " + std::string( sqlite3_errmsg( m_db ) ) );
		}

		Log::Info( "Old sessions deleted successfully." );
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

		return sqlite3_last_insert_rowid( m_db );
	}
	catch( const std::exception &e )
	{
		throw std::runtime_error( "Database error: " + std::string( e.what() ) );
	}

	return 0;
}

int64_t Database::VerifyAccount( const std::wstring &username, const std::wstring &password )
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

			if( username_utf8 == dbUsername && VerifyPassword( password_utf8, dbPassword ) )
			{
				Log::Debug( "Account verified: {} (ID: {})", username, accountId );
				return accountId;
			}
			else
			{
				Log::Error( "Invalid credentials for account ID: {}", accountId );
				return -1;
			}
		}
		else if( sqlite3_step( stmt ) == SQLITE_DONE )
		{
			return -1; // No matching account found
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

	return -1;
}

std::tuple< bool, std::wstring > Database::LoadAccount( const int64_t account_id )
{
	try
	{
		auto stmt = m_statements[ QueryID::LoadAccount ];

		sqlite3_reset( stmt );
		sqlite3_clear_bindings( stmt );

		sqlite3_bind_int64( stmt, 1, account_id );

		if( sqlite3_step( stmt ) == SQLITE_ROW )
		{
			const char *chatHandle = reinterpret_cast< const char * >( sqlite3_column_text( stmt, 0 ) );
			if( chatHandle )
			{
				Log::Debug( "Account loaded: ID: {}, Chat Handle: {}", account_id, chatHandle );
				return std::make_tuple( true, Util::UTF8ToWide( chatHandle ) );
			}
			else
			{
				Log::Error( "Chat handle not found for account ID: {}", account_id );
				return std::make_tuple( false, L"" );
			}
		}
		else if( sqlite3_step( stmt ) == SQLITE_DONE )
		{
			Log::Error( "No account found for ID: {}", account_id );
			return std::make_tuple( false, L"" );
		}
		else
		{
			throw std::runtime_error( "Query failed: " + std::string( sqlite3_errmsg( m_db ) ) );
		}

	}
	catch( const std::exception & )
	{

	}

	Log::Error( "Database error while loading account ID: {}", account_id );
	return std::make_tuple( false, L"" );
}

bool Database::CreateSession( const int64_t account_id, const std::wstring &session_id, const std::string &ip_address )
{
	try
	{
		auto stmt = m_statements[ QueryID::CreateSession ];
		auto sessionId = Util::WideToUTF8( session_id );

		sqlite3_reset( stmt );
		sqlite3_clear_bindings( stmt );

		sqlite3_bind_int64( stmt, 1, account_id );
		sqlite3_bind_text( stmt, 2, sessionId.c_str(), -1, SQLITE_TRANSIENT );
		sqlite3_bind_int( stmt, 3, 0 );
		sqlite3_bind_text( stmt, 4, ip_address.c_str(), -1, SQLITE_TRANSIENT );
		sqlite3_bind_int64( stmt, 5, time( nullptr ) + 300 ); // Session expires in 5 minutes

		if( sqlite3_step( stmt ) != SQLITE_DONE )
		{
			Log::Error( "SQLite insert failed: {}", sqlite3_errmsg( m_db ) );
			return false;
		}

		return true;
	}
	catch( const std::exception &e )
	{
		Log::Error( "Database error: {}", std::string( e.what() ) );
	}

	return false;
}

bool Database::UpdateSession( const std::wstring &session_id, const uint32_t character_id )
{
	try
	{
		auto stmt = m_statements[ QueryID::UpdateSession ];
		auto sessionId = Util::WideToUTF8( session_id );

		sqlite3_reset( stmt );
		sqlite3_clear_bindings( stmt );

		sqlite3_bind_int64( stmt, 1, time( nullptr ) + 300 ); // Extend session by 5 minutes
		sqlite3_bind_int( stmt, 2, character_id );
		sqlite3_bind_text( stmt, 3, sessionId.c_str(), -1, SQLITE_TRANSIENT );

		if( sqlite3_step( stmt ) != SQLITE_DONE )
		{
			Log::Error( "SQLite update failed: {}", sqlite3_errmsg( m_db ) );
			return false;
		}

		Log::Debug( "Session updated: {}", session_id );
		return true;
	}
	catch( const std::exception &e )
	{
		Log::Error( "Database error: {}", std::string( e.what() ) );
	}

	return false;
}

bool Database::DeleteSession( const std::wstring &session_id )
{
	try
	{
		auto stmt = m_statements[ QueryID::DeleteSession ];
		auto sessionId = Util::WideToUTF8( session_id );

		sqlite3_reset( stmt );
		sqlite3_clear_bindings( stmt );

		sqlite3_bind_text( stmt, 1, sessionId.c_str(), -1, SQLITE_TRANSIENT );

		if( sqlite3_step( stmt ) != SQLITE_DONE )
		{
			Log::Error( "SQLite delete failed: {}", sqlite3_errmsg( m_db ) );
			return false;
		}

		Log::Debug( "Session deleted: {}", session_id );
		return true;
	}
	catch( const std::exception &e )
	{
		Log::Error( "Database error: {}", std::string( e.what() ) );
	}
	return false;
}

std::tuple< int64_t, uint32_t > Database::GetSession( const std::wstring &session_id, const std::string &ip_address )
{
	try
	{
		auto stmt = m_statements[ QueryID::GetSession ];
		auto sessionId = Util::WideToUTF8( session_id );

		sqlite3_reset( stmt );
		sqlite3_clear_bindings( stmt );

		sqlite3_bind_text( stmt, 1, sessionId.c_str(), -1, SQLITE_TRANSIENT );
		sqlite3_bind_text( stmt, 2, ip_address.c_str(), -1, SQLITE_TRANSIENT );

		if( sqlite3_step( stmt ) == SQLITE_ROW )
		{
			uint64_t account_id = sqlite3_column_int64( stmt, 0 );
			uint32_t character_id = sqlite3_column_int( stmt, 1 );
			uint64_t expire_time = sqlite3_column_int64( stmt, 2 );

			if( expire_time > time( nullptr ) )
			{
				Log::Debug( "Session found: {} for account ID: {}", session_id, account_id );
				return std::make_tuple( account_id, character_id );
			}
		}
		else if( sqlite3_step( stmt ) == SQLITE_DONE )
		{
			Log::Error( "No session found for ID: {}", session_id );
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

	return std::make_tuple( -1, -1 );
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
