#include "../../global_define.h"
#include "RequestLogout.h"

void RequestLogout::Deserialize( sptr_tcp_socket socket, sptr_byte_stream stream )
{
	DeserializeHeader( stream );
	m_sessionId = stream->read_encrypted_utf16();
}

sptr_generic_response RequestLogout::ProcessRequest( sptr_tcp_socket socket, sptr_byte_stream stream )
{
	Deserialize( socket, stream );

	auto &userMng = RealmUserManager::Get();

	auto user = userMng.GetUser( m_sessionId );

	if( nullptr == user )
	{
		Log::Error( "RequestLogout::ProcessRequest() - User not found!" );
		return std::make_shared< ResultLogout >( this, 1 );
	}

	// TODO: Any other cleanup here?
	Log::Debug( "[%S] Logout", m_sessionId.c_str() );

	userMng.RemoveUser( m_sessionId );

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
