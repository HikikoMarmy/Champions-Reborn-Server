#include "RequestGetRoom.h"

#include "../../Game/RealmUserManager.h"
#include "../../Game/ChatRoomManager.h"

#include "../../logging.h"

void RequestGetRoom::Deserialize( sptr_byte_stream stream )
{
	DeserializeHeader( stream );

	m_sessionId = stream->read_encrypted_utf16();
	m_roomName = stream->read_utf16();
}

sptr_generic_response RequestGetRoom::ProcessRequest( sptr_socket socket, sptr_byte_stream stream )
{
	Deserialize( stream );

	const auto user = UserManager::Get().FindUserBySocket( socket );
	if( !user )
	{
		return std::make_shared< ResultGetRoom >( this, GENERAL_ERROR );
	}

	sptr_chat_room_session targetRoom = nullptr;

	if( m_roomName.empty() )
	{
		targetRoom = ChatRoomManager::Get().FindRoom( user->m_privateRoomId );
	}
	else
	{
		targetRoom = ChatRoomManager::Get().FindRoom( user->m_publicRoomId );
	}

	return std::make_shared< ResultGetRoom >( this, SUCCESS, targetRoom );
}

ResultGetRoom::ResultGetRoom( GenericRequest *request, int32_t reply, sptr_chat_room_session room ) : GenericResponse( *request )
{
	m_reply = reply;
	m_room = room;
}

void ResultGetRoom::Serialize( ByteBuffer &out ) const
{
	out.write_u16( m_packetId );
	out.write_u32( m_trackId );
	out.write_u32( m_reply );

	if( m_room )
	{
		out.write_utf16( m_room->m_name );
		out.write_utf16( m_room->m_banner );

		out.write_u32( static_cast< uint32_t >( m_room->m_members.size() ) );
			
		for( const auto &m : m_room->m_members )
		{
			const auto &member = m.lock();

			if( member )
			{
				out.write_utf16( member->m_chatHandle );
			}
			else
			{
				out.write_utf16( L"" );
			}
		}

		out.write_u32( static_cast< uint32_t >( m_room->m_moderators.size() ) );
		for( const auto &m : m_room->m_moderators )
		{
			if( auto member = m.lock() )
			{
				out.write_utf16( member->m_chatHandle );
			}
			else
			{
				out.write_utf16( L"" );
			}
		}
	}
}
