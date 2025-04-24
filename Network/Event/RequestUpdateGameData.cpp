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
	Deserialize( stream );

	auto gameSession = GameSessionManager::Get().FindGame( user->m_gameId );

	if( gameSession == nullptr )
	{
		Log::Error( "Game not found! [%S]", m_sessionId.c_str() );
		return std::make_shared< ResultUpdateGameData >( this );
	}

	if( m_gameData.size() != 256 )
	{
		Log::Error( "Game data size is not 256 bytes! [%S]", m_sessionId.c_str() );
		return std::make_shared< ResultUpdateGameData >( this );
	}

	gameSession->m_gameData = m_gameData;

	// Parse Game Data. Not the most ideal way to do this, but it works.
	char currentPlayers = 0;
	char maxPlayers = 0;
	char description[ 200 ] = { 0 };

	int result = sscanf( m_gameData.c_str(), " %hhd / %hhd :%199[^\r\n]", &currentPlayers, &maxPlayers, description );

	if( result >= 2 )
	{
		gameSession->m_currentPlayers = currentPlayers;
		gameSession->m_maximumPlayers = maxPlayers;
		gameSession->m_description = ( result == 3 ) ? description : "";  // Empty if not parsed
	}
	else
	{
		Log::Debug( "Failed to parse game info from: %s", m_gameData.c_str() );
	}

	std::string ip = std::string( m_gameData.c_str() + 220, 24 );
	user->m_localAddr = ip;

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
