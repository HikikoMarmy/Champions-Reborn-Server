#include "RequestEnterRoom.h"

#include "../../Game/RealmUserManager.h"
#include "../../Game/ChatRoomManager.h"
#include "../../logging.h"

void RequestEnterRoom::Deserialize( sptr_byte_stream stream )
{
	DeserializeHeader( stream );

	auto sessionId = stream->read_encrypted_utf16();
	m_roomName = stream->read_utf16();
}

sptr_generic_response RequestEnterRoom::ProcessRequest( sptr_socket socket, sptr_byte_stream stream )
{
	Deserialize( stream );

	const auto user = UserManager::Get().FindUserBySocket( socket );
	if( !user )
	{
		Log::Error( "User not found for socket!" );
		return std::make_shared< ResultEnterRoom >( this, GENERAL_ERROR, nullptr );
	}

	if( !ChatRoomManager::Get().JoinRoom( user, m_roomName ) )
	{
		Log::Error( "Failed to join room [{}] for user [{}]", m_roomName, user->m_username );
		return std::make_shared< ResultEnterRoom >( this, GENERAL_ERROR, nullptr );
	}

	const auto room = ChatRoomManager::Get().FindRoom( m_roomName );
	if( !room )
	{
		Log::Error( "Chat room [{}] not found after joining", m_roomName );
		return std::make_shared< ResultEnterRoom >( this, GENERAL_ERROR, nullptr );
	}

	return std::make_shared< ResultEnterRoom >( this, SUCCESS, room );
}

ResultEnterRoom::ResultEnterRoom( GenericRequest *request, int32_t reply, sptr_chat_room_session room ) : GenericResponse( *request )
{
	m_reply = reply;
	m_room = room;
}

void ResultEnterRoom::Serialize( ByteBuffer &out ) const
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
			if( auto member = m.lock() )
			{
				out.write_utf16( member->m_chatHandle );
			}
			else
			{
				out.write_utf16( L"Unknown" );
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
