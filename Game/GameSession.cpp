
#include "GameSession.h"

GameSession::GameSession()
{
	m_owner.reset();

	m_gameIndex = 0;
	m_type = GameType::Public;
	m_state = GameState::NotReady;
	m_currentPlayers = 0;
	m_maximumPlayers = 0;

	m_hostPort = 0;
	m_hostLocalAddr.clear();
	m_hostExternalAddr.clear();
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
	m_currentPlayers = 0;
	m_maximumPlayers = 0;

	m_hostPort = 0;
	m_hostLocalAddr.clear();
	m_hostExternalAddr.clear();
	m_gameName.clear();
	m_ownerName.clear();
	m_gameData.clear();
	m_description.clear();
}