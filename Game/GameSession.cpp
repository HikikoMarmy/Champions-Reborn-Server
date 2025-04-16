#include "../global_define.h"

GameSession::GameSession()
{
	m_owner.reset();

	m_gameIndex = 0;
	m_type = GameType::Public;
	m_state = GameState::NotReady;
	m_minimumLevel = 0;
	m_maximumLevel = 0;
	m_currentPlayers = 0;
	m_maximumPlayers = 0;

	m_gameAddress.clear();
	m_gameName.clear();
	m_ownerName.clear();
	m_gameData.clear();
	m_description.clear();
}

GameSession::~GameSession()
{
	m_owner.reset();

	m_gameIndex = 0;
	m_type = GameType::Public;
	m_state = GameState::NotReady;
	m_minimumLevel = 0;
	m_maximumLevel = 0;
	m_currentPlayers = 0;
	m_maximumPlayers = 0;

	m_gameAddress.clear();
	m_gameName.clear();
	m_ownerName.clear();
	m_gameData.clear();
	m_description.clear();
}