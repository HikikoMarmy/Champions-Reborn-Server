#include "../../global_define.h"
#include "RequestLogout.h"

void RequestLogout::Deserialize( sptr_byte_stream stream )
{
	DeserializeHeader( stream );
	m_sessionId = stream->read_encrypted_utf16();
}

sptr_generic_response RequestLogout::ProcessRequest( sptr_user user, sptr_byte_stream stream )
{
	Deserialize( stream );

	Log::Debug( "[%S] Logout", m_sessionId.c_str() );

	return std::make_shared< ResultLogout >( this, 0 );
}

ResultLogout::ResultLogout( GenericRequest *request, int32_t reply ) : GenericResponse( *request )
{
	m_reply = reply;
}

ByteStream &ResultLogout::Serialize()
{
	m_stream.write_u16( m_packetId );
	m_stream.write_u32( m_requestId );
	m_stream.write_u32( m_reply );

	return m_stream;
}
