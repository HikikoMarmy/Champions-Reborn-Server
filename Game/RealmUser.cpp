#include "../global_define.h"

RealmUser::RealmUser()
{
	sock = nullptr;

	m_clientType = RealmClientType::UNKNOWN;

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
		sock->flag.disconnected = true;
		sock.reset();
	}

	m_clientType = RealmClientType::UNKNOWN;

	m_sessionId = L"";

	m_localAddr = "";
	m_discoveryAddr = "";
	m_discoveryPort = 0;

	m_isHost = false;
	m_gameId = 0;
}