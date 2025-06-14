#include "RequestCreatePrivateRoom.h"

#include "../../Game/RealmUserManager.h"
#include "../../Game/ChatRoomManager.h"
#include "../../logging.h"

void RequestCreatePrivateRoom::Deserialize( sptr_byte_stream stream )
{
	DeserializeHeader( stream );
}

sptr_generic_response RequestCreatePrivateRoom::ProcessRequest( sptr_socket socket, sptr_byte_stream stream )
{
	Deserialize( stream );

	auto user = RealmUserManager::Get().FindUserBySocket( socket );
	if( user == nullptr )
	{
		return std::make_shared< ResultCreatePrivateRoom >( this );
	}

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

ByteBuffer&ResultCreatePrivateRoom::Serialize()
{
	m_stream.write_u16( m_packetId );
	m_stream.write_u32( m_trackId );
	m_stream.write_u32( 0 );

	return m_stream;
}
