#include "../global_define.h"

RealmUser::RealmUser()
{
	m_state = UserState::MainMenu;
	m_realmSocket = nullptr;
	m_discoverySocket = nullptr;
	m_sessionId = L"";
}

RealmUser::~RealmUser()
{
	m_state = UserState::MainMenu;

	if( m_realmSocket )
	{
		m_realmSocket->flag.disconnected = true;
		m_realmSocket.reset();
	}

	if( m_discoverySocket )
	{
		m_discoverySocket->flag.disconnected = true;
		m_discoverySocket.reset();
	}

	m_sessionId = L"";
}