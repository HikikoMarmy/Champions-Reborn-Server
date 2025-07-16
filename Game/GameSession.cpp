
#include "GameSession.h"

#include "RealmUser.h"
#include "../../logging.h"

GameSession::GameSession( uint32_t index ) : m_gameId( index )
{
	m_members.fill( std::weak_ptr< RealmUser >() );

	m_type = GameType::Public;
	m_state = GameState::NotReady;
	m_currentPlayers = 0;
	m_maximumPlayers = 4;

	m_difficulty = 0;
	m_gameMode = 0;
	m_mission = 0;
	m_unknown = 0;
	m_networkSave = 0;

	m_hostNatPort = 0;
	m_hostLocalPort = 0;
	m_hostLocalAddr.clear();
	m_hostExternalAddr.clear();
	m_gameName.clear();
	m_ownerName.clear();
	m_gameData.clear();
	m_description.clear();
}

GameSession::~GameSession()
{
	m_members.fill( std::weak_ptr< RealmUser >() );

	m_gameId = 0;
	m_type = GameType::Public;
	m_state = GameState::NotReady;
	m_currentPlayers = 0;
	m_maximumPlayers = 0;

	m_difficulty = 0;
	m_gameMode = 0;
	m_mission = 0;
	m_unknown = 0;
	m_networkSave = 0;

	m_hostNatPort = 0;
	m_hostLocalAddr.clear();
	m_hostExternalAddr.clear();
	m_gameName.clear();
	m_ownerName.clear();
	m_gameData.clear();
	m_description.clear();
}

bool GameSession::IsJoinable( sptr_user user ) const
{
	if( user )
	{
		for( const auto &m : m_members )
		{
			if( m.expired() )
				continue;

			const auto &member = m.lock();
			if( member == user )
			{
				return false;
			}
		}
	}

	return ( m_state == GameState::Open && m_currentPlayers < m_maximumPlayers );
}

sptr_user GameSession::GetOwner() const
{
	if( !m_members[ 0 ].expired() )
	{
		return m_members[ 0 ].lock();
	}

	return nullptr;
}

sptr_user GameSession::GetMember( int32_t index ) const
{
	if( index < 0 || index >= static_cast< int32_t >( m_members.size() ) )
		return nullptr;

	if( !m_members[ index ].expired() )
	{
		return m_members[ index ].lock();
	}

	return nullptr;
}

sptr_user GameSession::GetMemberBySessionId( const std::wstring &sessionId ) const
{
	for( const auto &m : m_members )
	{
		if( m.expired() )
			continue;

		const auto &member = m.lock();
		if( member->m_sessionId == sessionId )
		{
			return member;
		}
	}
	return nullptr;
}

std::vector<sptr_user> GameSession::GetMembers() const
{
	std::vector< sptr_user > members;
	for( const auto &m : m_members )
	{
		if( !m.expired() )
		{
			members.push_back( m.lock() );
		}
	}
	return members;
}

bool GameSession::AddMember( sptr_user user )
{
	if( !user ) return false;

	int8_t freeIndex = -1;

	for( int8_t i = 0; i < static_cast< int8_t >( m_members.size() ); ++i )
	{
		auto memberPtr = m_members[ i ].lock();

		if( memberPtr )
		{
			if( memberPtr->m_sessionId == user->m_sessionId )
				return false;
		}
		else if( freeIndex == -1 )
		{
			freeIndex = i;
		}
	}

	if( freeIndex == -1 )
	{
		Log::Error( "Game session is full! [{}]", m_gameName );
		return false;
	}

	user->m_gameId = m_gameId;

	m_members[ freeIndex ] = user;
	m_currentPlayers++;

	Log::Info( "Added user [{}] to game session [{}] at index {}",
			   user->m_username, m_gameName, freeIndex );

	return true;
}

bool GameSession::RemoveMember( sptr_user user )
{
	if( !user ) return false;

	for( int8_t i = 0; i < static_cast< int8_t >( m_members.size() ); ++i )
	{
		auto memberPtr = m_members[ i ].lock();
		if( memberPtr && memberPtr == user )
		{
			user->m_gameId = -1;
			m_members[ i ].reset();
			break;
		}
	}

	m_currentPlayers--;

	Log::Info( "Removed user [{}] from game session [{}]", user->m_username, m_gameName );

	return true;
}
