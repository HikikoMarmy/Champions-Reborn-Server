#include "NotifyClientReqConnect.h"

NotifyClientRequestConnect::NotifyClientRequestConnect( std::string clientIp, int32_t clientPort ) : GenericMessage( 0x3F )
{
	this->m_clientIp = std::move( clientIp );
	this->m_clientPort = clientPort;
}

ByteBuffer &NotifyClientRequestConnect::Serialize()
{
	m_stream.write_u16( m_packetId );
	m_stream.write_u32( 0 );

	m_stream.write_sz_utf8( m_clientIp );
	m_stream.write_u32( m_clientPort );

	return m_stream;
}
