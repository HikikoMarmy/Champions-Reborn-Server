#include "RequestSendRoomMessage.h"

#include "../../Game/RealmUserManager.h"
#include "../../Game/ChatRoomManager.h"

#include "NotifyRoomMessage.h"
#include "../../logging.h"

void RequestSendRoomMessage::Deserialize( sptr_byte_stream stream )
{
	DeserializeHeader( stream );

	auto sessionId = stream->read_encrypted_utf16();
	m_roomName = stream->read_utf16();
	m_message = stream->read_utf16();
}

sptr_generic_response RequestSendRoomMessage::ProcessRequest( sptr_socket socket, sptr_byte_stream stream )
{
	Deserialize( stream );

	const auto user = UserManager::Get().FindUserBySocket( socket );
	if( !user )
	{
		Log::Error( "User not found for socket!" );
		return std::make_shared< ResultSendRoomMessage >( this, GENERAL_ERROR );
	}

	const auto room = ChatRoomManager::Get().FindRoom( m_roomName );

	if( !room )
	{
		Log::Error( "Chat room not found: {}", m_roomName );
		return std::make_shared< ResultSendRoomMessage >( this, GENERAL_ERROR );
	}

	if( !room->IsMember( user ) )
	{
		Log::Error( "User [{}] is not a member of chat room [{}]", user->m_chatHandle, m_roomName );
		return std::make_shared< ResultSendRoomMessage >( this, GENERAL_ERROR );
	}

	NotifyRoomMessage msg( m_roomName, user->m_chatHandle, m_message );

	for( const auto &member : room->m_members )
	{
		auto memberUser = member.lock();
		if( !memberUser )
			continue; 

		if( memberUser->IsIgnored( user->m_chatHandle ) )
		{
			continue; // Skip sending to ignored users
		}

		memberUser->sock->send( msg );
	}

	return std::make_shared< ResultSendRoomMessage >( this, SUCCESS );
}

ResultSendRoomMessage::ResultSendRoomMessage( GenericRequest *request, int32_t reply ) : GenericResponse( *request )
{
	m_reply = reply;
}

void ResultSendRoomMessage::Serialize( ByteBuffer &out ) const
{
	out.write_u16( m_packetId );
	out.write_u32( m_trackId );
	out.write_u32( m_reply );
}
