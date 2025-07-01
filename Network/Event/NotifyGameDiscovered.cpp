#include "NotifyGameDiscovered.h"
#include "../../Game/RealmUser.h"

NotifyGameDiscovered::NotifyGameDiscovered( sptr_user user ) : GenericMessage( 0x3E )
{
	this->m_clientIp = user->m_discoveryAddr;
	this->m_clientPort = user->m_discoveryPort;
	this->m_clientType = user->m_gameType;
}

void NotifyGameDiscovered::Serialize( ByteBuffer &out ) const
{
	out.write_u16( m_packetId );
	out.write_u32( 0 );

	if( m_clientType == RealmGameType::RETURN_TO_ARMS )
		out.write_utf8( m_clientIp );
	else
		out.write_sz_utf8( m_clientIp );

	out.write_u32( m_clientPort );
}
