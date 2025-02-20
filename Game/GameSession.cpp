
#include "../global_define.h"

#include "../Lobby Server/Event/NotifyClientReqConnect.h"
#include "../Lobby Server/Event/NotifyGameDiscovered.h"

GameSession::GameSession()
{
	m_gameIndex = 0;
	m_state = GameState::NotReady;
	m_minimumLevel = 0;
	m_maximumLevel = 0;
	m_maximumPlayers = 0;

	m_gameLocation.clear();
	m_gameName.clear();
	m_ownerName.clear();
	m_gameData.clear();
}

GameSession::~GameSession()
{

}

sptr_user GameSession::GetHost()
{
	return m_userList[ 0 ];
}