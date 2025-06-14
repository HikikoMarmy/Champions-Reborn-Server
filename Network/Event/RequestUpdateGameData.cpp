#include "RequestUpdateGameData.h"

#include "../../Game/RealmUserManager.h"
#include "../../Game/GameSessionManager.h"
#include "../../Game/RealmUser.h"
#include "../../logging.h"

void RequestUpdateGameData::Deserialize( sptr_byte_stream stream )
{
	DeserializeHeader( stream );

	m_sessionId = stream->read_encrypted_utf16();
	m_gameData = stream->read_utf8();
}

sptr_generic_response RequestUpdateGameData::ProcessRequest( sptr_socket socket, sptr_byte_stream stream )
{
	Deserialize( stream );

	auto user = RealmUserManager::Get().FindUserBySocket( socket );

	if( user == nullptr )
	{
		Log::Error( "User not found! [%S]", m_sessionId.c_str() );
		return std::make_shared< ResultUpdateGameData >( this );
	}

	auto gameSession = GameSessionManager::Get().FindGame( user->m_gameId, user->m_gameType );

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

ByteBuffer &ResultUpdateGameData::Serialize()
{
	m_stream.write_u16( m_packetId );
	m_stream.write_u32( m_trackId );
	m_stream.write_u32( 0 );

	return m_stream;
}
