#include "ChatRoomSession.h"

ChatRoomSession::ChatRoomSession()
{
	m_type = GameType::Public;
	m_gameIndex = 0;
	m_gameName.clear();
}

ChatRoomSession::~ChatRoomSession()
{
	m_type = GameType::Public;
	m_gameIndex = 0;
	m_gameName.clear();
}