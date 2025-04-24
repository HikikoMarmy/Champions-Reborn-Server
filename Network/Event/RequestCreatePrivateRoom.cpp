#include "../../global_define.h"
#include "RequestCreatePrivateRoom.h"

void RequestCreatePrivateRoom::Deserialize( sptr_byte_stream stream )
{
	DeserializeHeader( stream );
}

sptr_generic_response RequestCreatePrivateRoom::ProcessRequest( sptr_user user, sptr_byte_stream stream )
{
	Deserialize( stream );

	auto sessionId = stream->read_encrypted_utf16();
	auto roomName = stream->read_utf16();

	auto result = ChatRoomManager::Get().CreatePrivateChatSession( user, roomName );

	if( !result )
	{
		Log::Error( "RequestCreatePrivateRoom::ProcessRequest() - Failed to create private room!" );
		return std::make_shared< ResultCreatePrivateRoom >( this );
	}

	return std::make_shared< ResultCreatePrivateRoom >( this );
}

ResultCreatePrivateRoom::ResultCreatePrivateRoom( GenericRequest *request ) : GenericResponse( *request )
{
}

ByteStream&ResultCreatePrivateRoom::Serialize()
{
	m_stream.write_u16( m_packetId );
	m_stream.write_u32( m_requestId );
	m_stream.write_u32( 0 );

	return m_stream;
}
