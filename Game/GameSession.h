#pragma once

#include <string>
#include <array>

class GameSession {
public:
	GameSession()
	{
		m_gameIndex = 0;
		m_minimumLevel = 0;
		m_maximumLevel = 0;

		m_hostSessionId = L"";
		m_gameName = L"";

		m_userList.fill( nullptr );
	}

	~GameSession()
	{

	}

	sptr_user GetOwner()
	{
		return m_userList[ 0 ];
	}

	sptr_user GetUser( const size_t index )
	{
		if( index < 0 || index >= m_userList.size() )
		{
			return nullptr;
		}

		return m_userList[ index ];
	}

	int32_t m_gameIndex;
	std::wstring m_hostSessionId;
	std::wstring m_gameName;

	int32_t m_minimumLevel;
	int32_t m_maximumLevel;

	// User Information
	std::array< sptr_user, 4 > m_userList;
};

typedef std::shared_ptr< GameSession > sptr_game_session;