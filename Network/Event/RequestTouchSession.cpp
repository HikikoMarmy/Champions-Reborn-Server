#include "RequestTouchSession.h"

#include "../../Game/RealmUserManager.h"
#include "../../logging.h"

void RequestTouchSession::Deserialize( sptr_byte_stream stream )
{
	DeserializeHeader( stream );

	m_sessionId = stream->read_encrypted_utf16();
}

sptr_generic_response RequestTouchSession::ProcessRequest( sptr_socket socket, sptr_byte_stream stream )
{
	Deserialize( stream );

	auto user = UserManager::Get().FindUserBySocket( socket );

	if( user == nullptr )
	{
		Log::Error( "User not found! [{}]", m_sessionId );
		return std::make_shared< ResultTouchSession >( this );
	}

	return std::make_shared< ResultTouchSession >( this );
}

ResultTouchSession::ResultTouchSession( GenericRequest *request ) : GenericResponse( *request )
{
	
}

void ResultTouchSession::Serialize( ByteBuffer &out ) const
{
	out.write_u16( m_packetId );
	out.write_u32( m_trackId );
	out.write_u32( 0 );
}
