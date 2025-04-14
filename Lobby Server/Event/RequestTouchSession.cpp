#include "../../global_define.h"
#include "RequestTouchSession.h"

void RequestTouchSession::Deserialize( sptr_byte_stream stream )
{
	DeserializeHeader( stream );

	m_sessionId = stream->read_encrypted_utf16();
}

sptr_generic_response RequestTouchSession::ProcessRequest( sptr_user user, sptr_byte_stream stream )
{
	Deserialize( stream );

	return std::make_shared< ResultTouchSession >( this );
}

ResultTouchSession::ResultTouchSession( GenericRequest *request ) : GenericResponse( *request )
{
	
}

ByteStream &ResultTouchSession::Serialize()
{
	m_stream.write_u16( m_packetId );
	m_stream.write_u32( m_requestId );
	m_stream.write_u32( 0 );

	return m_stream;
}
