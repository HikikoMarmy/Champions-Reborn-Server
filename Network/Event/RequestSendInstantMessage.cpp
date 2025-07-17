#include "RequestSendInstantMessage.h"

#include "NotifyInstantMessage.h"
#include "../../Game/RealmUserManager.h"
#include "../../Game/ChatRoomManager.h"
#include "../../logging.h"

void RequestSendInstantMessage::Deserialize( sptr_byte_stream stream )
{
	DeserializeHeader( stream );

	auto sessionId = stream->read_encrypted_utf16();
	m_chatHandle = stream->read_utf16();
	m_message = stream->read_utf16();
}

sptr_generic_response RequestSendInstantMessage::ProcessRequest( sptr_socket socket, sptr_byte_stream stream )
{
	Deserialize( stream );

	const auto user = UserManager::Get().FindUserBySocket( socket );
	if( !user )
	{
		Log::Error( "User not found for socket!" );
		return std::make_shared< ResultSendInstantMessage >( this, GENERAL_ERROR );
	}

	auto targetUser = UserManager::Get().FindUserByChatHandle( m_chatHandle );
	if( !targetUser )
	{
		return std::make_shared< ResultSendInstantMessage >( this, GENERAL_ERROR );
	}

	if( targetUser->IsIgnored( user->m_chatHandle ) )
	{
		return std::make_shared< ResultSendInstantMessage >( this, USER_IGNORED );
	}

	targetUser->sock->send( NotifyInstantMessage( user->m_chatHandle, m_message ) );

	return std::make_shared< ResultSendInstantMessage >( this, SUCCESS );
}

ResultSendInstantMessage::ResultSendInstantMessage( GenericRequest *request, int32_t reply ) : GenericResponse( *request )
{
	m_reply = reply;
}

void ResultSendInstantMessage::Serialize( ByteBuffer &out ) const
{
	out.write_u16( m_packetId );
	out.write_u32( m_trackId );
	out.write_u32( m_reply );
}
