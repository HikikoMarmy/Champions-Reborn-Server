#include "NotifyClientDiscovered_RTA.h"

#include "../../Common/Constant.h"

NotifyClientDiscovered_RTA::NotifyClientDiscovered_RTA( std::string clientIp, int32_t clientPort ) : GenericMessage( 0x40 )
{
	this->m_clientIp = std::move( clientIp );
	this->m_clientPort = clientPort;
}

ByteBuffer & NotifyClientDiscovered_RTA::Serialize()
{
	m_stream.write_u16( m_packetId );
	m_stream.write_u32( 0 );

	m_stream.write_utf8( m_clientIp );
	m_stream.write_u32( m_clientPort );

	return m_stream;
}
