#include "../../global_define.h"
#include "NotifyClientDiscovered.h"

NotifyClientDiscovered::NotifyClientDiscovered( std::string clientIp, int32_t clientPort ) : GenericMessage( 0x40 )
{
	m_clientIp = clientIp;
	m_clientPort = clientPort;
}

ByteStream &NotifyClientDiscovered::Serialize()
{
	m_stream.write_u16( m_packetId );
	m_stream.write_sz_utf8( m_clientIp );
	m_stream.write_u32( m_clientPort );

	return m_stream;
}
