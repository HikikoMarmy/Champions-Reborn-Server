#pragma once

class GameSession {
public:
	GameSession();
	~GameSession();

	sptr_user GetHost();

	bool JoinUser( sptr_user user );

public:
	enum class GameType
	{
		Public,
		Private
	} m_type;

	enum class GameState
	{
		NotReady,
		Open,
		Started
	} m_state;

	int32_t m_gameIndex;
	std::wstring m_gameLocation;
	std::wstring m_gameName;
	std::wstring m_ownerName;

	std::string m_gameData;

	int32_t m_maximumPlayers;
	int32_t m_minimumLevel;
	int32_t m_maximumLevel;

	// User Information
	std::vector< sptr_user >  m_userList;
};

typedef std::shared_ptr< GameSession > sptr_game_session;