#include "../../global_define.h"
#include "RequestCancelGame.h"

void RequestCancelGame::Deserialize( sptr_byte_stream stream )
{
	DeserializeHeader( stream );

	m_sessionId = stream->read_encrypted_utf16();
}

sptr_generic_response RequestCancelGame::ProcessRequest( sptr_user user, sptr_byte_stream stream )
{
	Deserialize( stream );

	if( user == nullptr )
	{
		Log::Error( "User not found! [%S]", m_sessionId.c_str() );
		return std::make_shared< ResultCancelGame >( this );
	}

	GameSessionManager::Get().CancelGameSession( user );

	// TODO:
	// Notify the players via the Discovery Server

	return std::make_shared< ResultCancelGame >( this );
}

ResultCancelGame::ResultCancelGame( GenericRequest *request ) : GenericResponse( *request )
{
	
}

ByteStream &ResultCancelGame::Serialize()
{
	m_stream.write_u16( m_packetId );
	m_stream.write_u32( m_requestId );
	m_stream.write_u32( 0 );

	return m_stream;
}
