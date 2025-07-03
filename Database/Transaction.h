#pragma once

#include <sqlite3.h>
#include <stdexcept>

class SQLiteTransaction {
	sqlite3 *m_db = nullptr;
	bool m_committed = false;

public:
	explicit SQLiteTransaction( sqlite3 *db )
		: m_db( db )
	{
		if( !m_db )
			throw std::invalid_argument( "Null SQLite database pointer" );

		if( sqlite3_exec( m_db, "BEGIN TRANSACTION;", nullptr, nullptr, nullptr ) != SQLITE_OK )
			throw std::runtime_error( "Failed to begin transaction: " + std::string( sqlite3_errmsg( m_db ) ) );
	}

	~SQLiteTransaction()
	{
		if( m_committed || !m_db )
			return;

		sqlite3_exec( m_db, "ROLLBACK;", nullptr, nullptr, nullptr );
	}

	SQLiteTransaction( const SQLiteTransaction & ) = delete;
	SQLiteTransaction &operator=( const SQLiteTransaction & ) = delete;

	SQLiteTransaction( SQLiteTransaction &&other ) noexcept
		: m_db( other.m_db ), m_committed( other.m_committed )
	{
		other.m_db = nullptr;
	}

	SQLiteTransaction &operator=( SQLiteTransaction &&other ) noexcept
	{
		if( this != &other )
		{
			commitOrRollback();
			m_db = other.m_db;
			m_committed = other.m_committed;
			other.m_db = nullptr;
		}
		return *this;
	}

	void commit()
	{
		if( m_committed || !m_db )
			return;

		if( sqlite3_exec( m_db, "COMMIT;", nullptr, nullptr, nullptr ) != SQLITE_OK )
			throw std::runtime_error( "Failed to commit transaction: " + std::string( sqlite3_errmsg( m_db ) ) );
		m_committed = true;
	}

private:
	void commitOrRollback()
	{
		if( m_committed || !m_db )
			return;

		sqlite3_exec( m_db, "ROLLBACK;", nullptr, nullptr, nullptr );
	}
};
