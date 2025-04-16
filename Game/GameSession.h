#pragma once

class GameSession {
public:
	GameSession();
	~GameSession();

	enum class GameType
	{
		Public,
		Private
	} m_type;

	enum class GameState
	{
		NotReady,
		Open
	} m_state;

	std::weak_ptr< RealmUser > m_owner;

	int32_t m_gameIndex;
	
	std::wstring m_gameAddress;
	std::wstring m_gameName;
	std::wstring m_ownerName;

	std::string m_gameData;
	std::string m_description;
	int8_t m_currentPlayers;
	int8_t m_maximumPlayers;
	int32_t m_minimumLevel;
	int32_t m_maximumLevel;
};

typedef std::shared_ptr< GameSession > sptr_game_session;