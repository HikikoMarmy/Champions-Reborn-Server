#include "../../global_define.h"
#include "RequestUpdateGameData.h"

void RequestUpdateGameData::Deserialize( sptr_byte_stream stream )
{
	DeserializeHeader( stream );

	m_sessionId = stream->read_encrypted_utf16();
	m_gameData = stream->read_utf8();
}

sptr_generic_response RequestUpdateGameData::ProcessRequest( sptr_user user, sptr_byte_stream stream )
{
	Deserialize( stream);

	Log::Packet( stream->data, stream->data.size(), false );

	auto gameSession = GameSessionManager::Get().FindGame( user->game_id );

	if( gameSession == nullptr )
	{
		Log::Error( "Game not found! [%S]", m_sessionId.c_str() );
		return std::make_shared< ResultUpdateGameData >( this );
	}

	gameSession->m_gameData = m_gameData;

	return std::make_shared< ResultUpdateGameData >( this );
}

ResultUpdateGameData::ResultUpdateGameData( GenericRequest *request ) : GenericResponse( *request )
{
	
}

ByteStream &ResultUpdateGameData::Serialize()
{
	m_stream.write_u16( m_packetId );
	m_stream.write_u32( m_requestId );
	m_stream.write_u32( 0 );

	return m_stream;
}
