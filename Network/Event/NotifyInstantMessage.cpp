#include "NotifyInstantMessage.h"

NotifyInstantMessage::NotifyInstantMessage( std::wstring chatHandle, std::wstring message ) : GenericMessage( 0x30 )
{
	m_chatHandle = chatHandle;
	m_message = message;
}

void NotifyInstantMessage::Serialize( ByteBuffer &out ) const
{
	out.write_u16( m_packetId );
	out.write_u32( 0 );

	out.write_utf16( m_chatHandle );
	out.write_utf16( m_message );
}
