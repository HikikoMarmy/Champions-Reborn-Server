#include "NotifyGameDiscovered.h"

NotifyGameDiscovered::NotifyGameDiscovered( std::string clientIp, int32_t clientPort, RealmGameType clientType ) : GenericMessage( 0x3E )
{
	this->m_clientIp = std::move( clientIp );
	this->m_clientPort = clientPort;
	this->m_clientType = clientType;
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
