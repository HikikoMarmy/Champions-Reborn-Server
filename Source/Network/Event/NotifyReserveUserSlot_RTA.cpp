#include "NotifyReserveUserSlot_RTA.h"

NotifyReserveUserSlot_RTA::NotifyReserveUserSlot_RTA( int32_t memberId, std::string discoveryAddr, int32_t discoveryPort ) : GenericMessage( 0x51 )
{
	this->m_discoveryAddr = discoveryAddr;
	this->m_port = discoveryPort;
	this->m_memberId = memberId;
}

// This notification is nullsub.
void NotifyReserveUserSlot_RTA::Serialize( ByteBuffer &out ) const
{
	out.write_u16( m_packetId );
	out.write_u32( 0 );

	out.write_u32( 0 );	
	out.write_utf8( "" );
	out.write_u32( 0 );	
}
