#include "RequestGetPublicRooms.h"

#include "../../Game/ChatRoomManager.h"

void RequestGetPublicRooms::Deserialize( sptr_byte_stream stream )
{
	DeserializeHeader( stream );
}

sptr_generic_response RequestGetPublicRooms::ProcessRequest( sptr_socket socket, sptr_byte_stream stream )
{
	Deserialize( stream );

	const auto publicRooms = ChatRoomManager::Get().GetPublicRoomList();

	return std::make_shared< ResultGetPublicRooms >( this, publicRooms );
}

ResultGetPublicRooms::ResultGetPublicRooms( GenericRequest *request, std::vector< sptr_chat_room_session > rooms ) : GenericResponse( *request )
{
	m_rooms = std::move( rooms );
}

void ResultGetPublicRooms::Serialize( ByteBuffer &out ) const
{
	out.write_u16( m_packetId );
	out.write_u32( m_trackId );
	out.write_u32( 0 );

	const auto numRoom = static_cast< uint32_t >( m_rooms.size() );

	out.write_u32( numRoom );
	for( const auto &room : m_rooms )
	{
		out.write_utf16( room->m_name );
	}

	out.write_u32( numRoom );
	for( const auto &room : m_rooms )
	{
		out.write_utf16( L"UNKNOWN" );
	}

	out.write_u32( numRoom );
	for( const auto &room : m_rooms )
	{
		out.write_u32( 88 );
	}
	
	out.write_u32( numRoom );
	for( const auto &room : m_rooms )
	{
		out.write_u32( 99 );
	}
}
