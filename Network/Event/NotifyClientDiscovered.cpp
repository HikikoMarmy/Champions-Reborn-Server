#include "../../global_define.h"
#include "NotifyClientDiscovered.h"

NotifyClientDiscovered::NotifyClientDiscovered( std::string clientIp, int32_t clientPort, RealmClientType clientType ) : GenericMessage( 0x40 )
{
	this->m_clientIp = std::move( clientIp );
	this->m_clientPort = clientPort;
	this->m_clientType = clientType;
}

ByteStream &NotifyClientDiscovered::Serialize()
{
	m_stream.write_u16( m_packetId );
	m_stream.write_u32( 0 );

	if( m_clientType == RealmClientType::RETURN_TO_ARMS )
		m_stream.write_utf8( m_clientIp );
	else
		m_stream.write_sz_utf8( m_clientIp );

	m_stream.write_u32( m_clientPort );

	return m_stream;
}
