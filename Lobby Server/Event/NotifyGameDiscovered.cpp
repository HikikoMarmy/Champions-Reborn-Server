#include "../../global_define.h"
#include "NotifyGameDiscovered.h"

NotifyGameDiscovered::NotifyGameDiscovered( std::string clientIp, int32_t clientPort ) : GenericMessage( 0x3E )
{
	m_clientIp = clientIp;
	m_clientPort = clientPort;
}

ByteStream &NotifyGameDiscovered::Serialize()
{
	m_stream.write_u16( m_packetId );
	m_stream.write_sz_utf8( m_clientIp );
	m_stream.write_u32( m_clientPort );

	return m_stream;
}
