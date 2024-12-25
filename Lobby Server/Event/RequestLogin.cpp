#include "../../global_define.h"

#include "RequestLogin.h"

void RequestLogin::Deserialize( sptr_tcp_socket socket, sptr_byte_stream stream )
{
	DeserializeHeader( stream );

	m_username = stream->read_encrypted_utf16();
	m_password = stream->read_encrypted_utf16();
}

sptr_generic_response RequestLogin::ProcessRequest( sptr_tcp_socket socket, sptr_byte_stream stream )
{
	Deserialize( socket, stream );

	if( m_username.empty() || m_password.empty() )
	{
		Log::Error( "RequestLogin::ProcessRequest() - Username or password is empty" );
		return std::make_shared< ResultLogin >( this, LOGIN_REPLY::NOT_EXIST, L"" );
	}

	if( m_username == L"foo" && m_password == L"bar" )
	{
		// Retail CoN does not use any login information.
		Log::Debug( "RequestLogin : Champions of Norrath v2.0" );
	}
	else
	{
		// Network Beta CoN uses login information, but it's invalid because of version 2.0.
		Log::Debug( "RequestLogin : Champions of Norrath v1.0" );
	}

	auto &userMng = RealmUserManager::Get();

	auto user = userMng.CreateUser( socket, m_username, m_password );

	return std::make_shared< ResultLogin >( this, LOGIN_REPLY::SUCCESS, user->m_sessionId );
}

ResultLogin::ResultLogin( GenericRequest *request, int32_t reply, std::wstring sessionId ) : GenericResponse( *request )
{
	m_reply = reply;
	m_sessionId = sessionId;
}

ByteStream &ResultLogin::Serialize()
{
	m_stream.write_u16( m_packetId );
	m_stream.write_u32( m_requestId );
	m_stream.write_u32( m_reply );

	m_stream.write_encrypted_utf16( m_sessionId );

	return m_stream;
}
