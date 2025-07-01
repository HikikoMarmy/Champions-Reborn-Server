#include "NotifyForcedLogout.h"

NotifyForcedLogout::NotifyForcedLogout() : GenericMessage( 0x41 )
{
}

void NotifyForcedLogout::Serialize( ByteBuffer &out ) const
{
	out.write_u16( m_packetId );
	out.write_u32( 0 );
	out.write_u32( 0 );
}
