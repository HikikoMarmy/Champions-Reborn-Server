#include "RequestCreatePrivateRoom.h"

#include "../../Game/RealmUserManager.h"
#include "../../Game/ChatRoomManager.h"
#include "../../logging.h"

void RequestCreatePrivateRoom::Deserialize( sptr_byte_stream stream )
{
	DeserializeHeader( stream );

	m_sessionId = stream->read_encrypted_utf16();
	m_roomName = stream->read_utf16();
}

sptr_generic_response RequestCreatePrivateRoom::ProcessRequest( sptr_socket socket, sptr_byte_stream stream )
{
	Deserialize( stream );

	auto user = UserManager::Get().FindUserBySocket( socket );
	if( user == nullptr )
	{
		return std::make_shared< ResultCreatePrivateRoom >( this );
	}

	auto result = ChatRoomManager::Get().CreateGameChatSession( user, m_roomName );

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

void ResultCreatePrivateRoom::Serialize( ByteBuffer &out ) const
{
	out.write_u16( m_packetId );
	out.write_u32( m_trackId );
	out.write_u32( 0 );
}
