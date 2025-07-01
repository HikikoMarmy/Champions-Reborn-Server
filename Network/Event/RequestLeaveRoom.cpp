#include "RequestLeaveRoom.h"

#include "../../Game/RealmUserManager.h"
#include "../../Game/ChatRoomManager.h"
#include "../../logging.h"

void RequestLeaveRoom::Deserialize( sptr_byte_stream stream )
{
	DeserializeHeader( stream );

	auto sessionId = stream->read_encrypted_utf16();
	m_roomName = stream->read_utf16();
}

sptr_generic_response RequestLeaveRoom::ProcessRequest( sptr_socket socket, sptr_byte_stream stream )
{
	Deserialize( stream );

	const auto user = UserManager::Get().FindUserBySocket( socket );
	if( !user )
	{
		Log::Error( "User not found for socket!" );
		return std::make_shared< ResultLeaveRoom >( this, GENERAL_ERROR );
	}

	if( !ChatRoomManager::Get().LeaveRoom( user, m_roomName ) )
	{
		Log::Error( "Could not remove user [{}] from room [{}]", user->m_username, m_roomName );
		return std::make_shared< ResultLeaveRoom >( this, GENERAL_ERROR );
	}

	return std::make_shared< ResultLeaveRoom >( this, SUCCESS );
}

ResultLeaveRoom::ResultLeaveRoom( GenericRequest *request, int32_t reply ) : GenericResponse( *request )
{
	m_reply = reply;
}

void ResultLeaveRoom::Serialize( ByteBuffer &out ) const
{
	out.write_u16( m_packetId );
	out.write_u32( m_trackId );
	out.write_u32( m_reply );
}
