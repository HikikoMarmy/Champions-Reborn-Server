#pragma once

#include <array>
#include <vector>
#include <string>
#include <memory>

#include "RealmUser.h"

class GameSession {
public:
	enum class GameType {
		Public,
		Private
	};

	enum class GameState {
		NotReady,
		Open,
		Started
	};

	GameSession( uint32_t index );
	~GameSession();

	bool IsJoinable( sptr_user user = nullptr ) const;

	sptr_user GetOwner() const;
	sptr_user GetMember( int32_t index ) const;
	sptr_user GetMemberBySessionId( const std::wstring &sessionId ) const;
	std::vector< sptr_user > GetMembers() const;
	bool AddMember( sptr_user user );
	bool RemoveMember( sptr_user user );

public:
	GameType m_type;
	GameState m_state;

	std::array< wptr_user, 4 > m_members;

	int32_t m_gameId;
	
	std::wstring m_gameName;
	std::wstring m_ownerName;
	std::wstring m_playerCount;

	std::string	m_gameData;
	std::string	m_description;

	std::string m_hostLocalAddr;
	std::string m_hostExternalAddr;
	int32_t m_hostLocalPort;
	int32_t m_hostNatPort;

	int8_t m_currentPlayers;
	int8_t m_maximumPlayers;

	int8_t m_difficulty;
	int8_t m_gameMode;
	int8_t m_mission;
	int8_t m_unknown;
	int8_t m_networkSave;
};

typedef std::shared_ptr< GameSession > sptr_game_session;