#include "RequestLogout.h"

#include "../../Game/RealmUserManager.h"
#include "../../logging.h"

void RequestLogout::Deserialize( sptr_byte_stream stream )
{
	DeserializeHeader( stream );
	m_sessionId = stream->read_encrypted_utf16();
}

sptr_generic_response RequestLogout::ProcessRequest( sptr_socket socket, sptr_byte_stream stream )
{
	Deserialize( stream );

	UserManager::Get().RemoveUser( socket );

	Log::Debug( "[{}] Logout", m_sessionId );

	return std::make_shared< ResultLogout >( this, 0 );
}

ResultLogout::ResultLogout( GenericRequest *request, int32_t reply ) : GenericResponse( *request )
{
	m_reply = reply;
}

void ResultLogout::Serialize( ByteBuffer &out ) const
{
	out.write_u16( m_packetId );
	out.write_u32( m_trackId );
	out.write_u32( m_reply );
}
