#include "NotifyClientRequestConnect_RTA.h"

NotifyClientRequestConnect_RTA::NotifyClientRequestConnect_RTA( std::string localAddr, std::string remoteAddr, int32_t port ) : GenericMessage( 0x65 )
{
	this->m_localAddr = localAddr;
	this->m_remoteAddr = remoteAddr;
	this->m_port = port;
}

ByteBuffer &NotifyClientRequestConnect_RTA::Serialize()
{
	m_stream.write_u16( m_packetId );
	m_stream.write_u32( 0 );

	m_stream.write_utf8( this->m_localAddr);
	m_stream.write_u32( this->m_port );

	m_stream.write_utf8( this->m_localAddr );
	m_stream.write_u32( this->m_port );

	return m_stream;
}