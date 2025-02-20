#include "../../global_define.h"
#include "RequestGetGame.h"

void RequestGetGame::Deserialize( sptr_byte_stream stream )
{
	DeserializeHeader( stream );

	m_sessionId = stream->read_encrypted_utf16();
	m_gameName = stream->read_utf16();
}

sptr_generic_response RequestGetGame::ProcessRequest( sptr_user user, sptr_byte_stream stream )
{
	Deserialize( stream );

	Log::Debug( "RequestGetGame : %S", m_sessionId.c_str() );
	Log::Debug( "RequestGetGame : %S", m_gameName.c_str() );

	if( user == nullptr )
	{
		Log::Error( "User not found! [%S]", m_sessionId.c_str() );
		return std::make_shared< ResultGetGame >( this );
	}

	user->is_host = false;
	user->is_ready = false;
	user->game_id = -1;

	return std::make_shared< ResultGetGame >( this );
}

ResultGetGame::ResultGetGame( GenericRequest *request ) : GenericResponse( *request )
{
	
}

ByteStream & ResultGetGame::Serialize()
{
	m_stream.write_u16( m_packetId );
	m_stream.write_u32( m_requestId );
	m_stream.write_u32( 0 );

	m_stream.write_utf16( L"Kelethin" );
	m_stream.write_utf16(L"OwnerName");
	m_stream.write_u32(0);

	return m_stream;
}
