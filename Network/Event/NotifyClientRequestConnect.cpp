#include "NotifyClientRequestConnect.h"

#include "../../Game/RealmUser.h"

NotifyClientRequestConnect::NotifyClientRequestConnect( sptr_user user ) : GenericMessage( 0x3F )
{
	this->m_clientIp = user->m_discoveryAddr;
	this->m_clientPort = user->m_discoveryPort;
}

void NotifyClientRequestConnect::Serialize( ByteBuffer &out ) const
{
	out.write_u16( m_packetId );
	out.write_u32( 0 );

	out.write_sz_utf8( m_clientIp );
	out.write_u32( m_clientPort );
}
