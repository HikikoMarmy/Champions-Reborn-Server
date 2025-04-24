#include "../../global_define.h"
#include "NotifyForcedLogout.h"

NotifyForcedLogout::NotifyForcedLogout() : GenericMessage( 0x41 )
{
}

ByteStream &NotifyForcedLogout::Serialize()
{
	m_stream.write_u16( m_packetId );
	m_stream.write_u32( 0 );

	return m_stream;
}
