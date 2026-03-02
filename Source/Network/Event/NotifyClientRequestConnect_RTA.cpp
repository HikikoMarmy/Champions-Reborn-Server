#include "Network\Event\NotifyClientRequestConnect_RTA.hpp"

#include "Game/RealmUser.hpp"

NotifyClientRequestConnect_RTA::NotifyClientRequestConnect_RTA( sptr_user user ) : GenericMessage( 0x65 )
{
	this->m_remoteAddr = user->m_discoveryAddr;
	this->m_remotePort = user->m_discoveryPort;

	this->m_localAddr = user->m_localAddr;
	this->m_localPort = user->m_localPort;

}

void NotifyClientRequestConnect_RTA::Serialize( ByteBuffer &out ) const
{
	out.write_u16( m_packetId );
	out.write_u32( 0 );

	out.write_utf8( this->m_localAddr );
	out.write_u32( this->m_localPort );

	out.write_utf8( this->m_remoteAddr );
	out.write_u32( this->m_remotePort );
}