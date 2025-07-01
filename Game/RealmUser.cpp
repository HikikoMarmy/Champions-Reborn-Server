#include "RealmUser.h"
#include "RealmCharacter.h"

RealmUser::RealmUser()
{
	sock = nullptr;

	m_gameType = RealmGameType::CHAMPIONS_OF_NORRATH;

	m_accountId = -1;
	m_sessionId = L"";
	m_username = L"";
	m_characterId = 0;

	m_localAddr = "";
	m_discoveryAddr = "";
	m_discoveryPort = 0;

	m_isLoggedIn = false;
	m_isHost = false;
	m_memberId = -1;
	m_gameId = -1;
	m_publicRoomId = -1;
	m_privateRoomId = -1;
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
	m_sessionId = L"";
	m_username = L"";
	m_characterId = 0;

	m_localAddr = "";
	m_discoveryAddr = "";
	m_discoveryPort = 0;

	m_isLoggedIn = false;
	m_isHost = false;
	m_memberId = 0;
	m_gameId = 0;
	m_publicRoomId = -1;
	m_privateRoomId = -1;
}
