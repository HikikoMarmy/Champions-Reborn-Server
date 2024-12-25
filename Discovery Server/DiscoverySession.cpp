#include "../global_define.h"
#include "DiscoverySession.h"

DiscoverySession::DiscoverySession( sptr_udp_socket socket )
{
	m_socket = socket;
	m_ownerSessionId = L"";
	m_userSessionIds.fill( L"" );
}

DiscoverySession::~DiscoverySession()
{
	m_socket.reset();
	m_ownerSessionId.clear();
	m_userSessionIds.fill( L"" );
}

sptr_discovery_record DiscoverySession::Create( sptr_udp_socket socket )
{
	return std::shared_ptr<DiscoverySession>( new DiscoverySession( socket ) );
}
