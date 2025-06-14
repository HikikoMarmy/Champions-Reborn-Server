#include "NotifyReserveUserSlot_RTA.h"

NotifyReserveUserSlot_RTA::NotifyReserveUserSlot_RTA( std::string discoveryAddr, int32_t discoveryPort ) : GenericMessage( 0x51 )
{
	this->m_discoveryAddr = discoveryAddr;
	this->m_port = discoveryPort;
}

ByteBuffer &NotifyReserveUserSlot_RTA::Serialize()
{
	m_stream.write_u16( m_packetId );
	m_stream.write_u32( 0 );

	m_stream.write_u32( 0 ); // Maybe slot or character ID?
	m_stream.write_utf8(""); // Probably IP
	m_stream.write_u32( 0 ); // Probably Port

	return m_stream;
}
