#include "RealmUser.h"

RealmUser::RealmUser()
{
	sock = nullptr;

	m_gameType = RealmGameType::CHAMPIONS_OF_NORRATH;

	m_accountId = -1;
	m_characterId = -1;
	m_sessionId = L"";

	m_localAddr = "";
	m_discoveryAddr = "";
	m_discoveryPort = 0;

	m_isHost = false;
	m_gameId = 0;
}

RealmUser::~RealmUser()
{
	if( sock )
	{
		sock->flag.disconnected_wait = true;
		sock.reset();
	}

	m_gameType = RealmGameType::CHAMPIONS_OF_NORRATH;

	m_accountId = 0;
	m_characterId = 0;
	m_sessionId = L"";

	m_localAddr = "";
	m_discoveryAddr = "";
	m_discoveryPort = 0;

	m_isHost = false;
	m_gameId = 0;
}