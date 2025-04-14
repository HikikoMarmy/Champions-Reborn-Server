#include "../global_define.h"

RealmUser::RealmUser()
{
	sock = nullptr;

	m_sessionId = L"";

	m_localAddr = "";
	m_discoveryAddr = "";

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

	m_sessionId = L"";

	m_localAddr = "";
	m_discoveryAddr = "";

	m_isHost = false;
	m_gameId = 0;
}