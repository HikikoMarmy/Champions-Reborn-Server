#include "NotifyFriendStatus.h"

NotifyFriendStatus::NotifyFriendStatus( std::wstring handle, bool status ) : GenericMessage( 0x2F )
{
	m_handle = handle;
	m_status = status;
}

void NotifyFriendStatus::Serialize( ByteBuffer &out ) const
{
	out.write_u16( m_packetId );
	out.write_u32( 0 );

	out.write_utf16( m_handle );
	out.write_u8( m_status ? 1 : 0 );
}
