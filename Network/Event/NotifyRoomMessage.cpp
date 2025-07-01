#include "NotifyRoomMessage.h"

NotifyRoomMessage::NotifyRoomMessage( std::wstring roomName, std::wstring chatHandle, std::wstring message ) : GenericMessage( 0x3D )
{
	m_roomName = roomName;
	m_chatHandle = chatHandle;
	m_message = message;
}

void NotifyRoomMessage::Serialize( ByteBuffer &out ) const
{
	out.write_u16( m_packetId );
	out.write_u32( 0 );

	out.write_utf16( m_roomName );
	out.write_utf16( m_chatHandle );
	out.write_utf16( m_message );
}
