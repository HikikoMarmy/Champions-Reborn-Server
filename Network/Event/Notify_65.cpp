#include "Notify_65.h"

NotifyClientRequestsConnect2::NotifyClientRequestsConnect2( std::string discoveryAddr, std::string localAddr, int32_t port ) : GenericMessage( 0x65 )
{
	m_discoveryAddr = std::move( discoveryAddr );
	m_localAddr = std::move( localAddr );
	m_port = port;
}

ByteBuffer &NotifyClientRequestsConnect2::Serialize()
{
	m_stream.write_u16( m_packetId );
	m_stream.write_u32( 0 );

	m_stream.write_utf8( m_discoveryAddr );
	m_stream.write_u32( m_port );

	m_stream.write_utf8( m_localAddr );
	m_stream.write_u32( m_port );

	return m_stream;
}
