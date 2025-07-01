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

bool RequestUpdateGameData::ParseGameData( sptr_game_session session )
{
	if( session == nullptr || m_gameData.size() < 256 )
	{
		Log::Error( "Invalid game session or game data size! [{}]", m_sessionId );
		return false;
	}

	session->m_gameData = m_gameData;

	int8_t currentPlayers = 0;
	int8_t maxPlayers = 0;
	char description[ 200 ] = { 0 };

	int result = sscanf( m_gameData.c_str(), " %hhd / %hhd :%199[^\r\n]", &currentPlayers, &maxPlayers, description );

	if( result >= 2 )
	{
		session->m_currentPlayers = currentPlayers;
		session->m_maximumPlayers = maxPlayers;
		session->m_description = ( result == 3 ) ? description : "";
	}
	else
	{
		Log::Debug( "Failed to parse game info from: {}", m_gameData );
	}

	return true;
}

sptr_generic_response RequestUpdateGameData::ProcessRequest( sptr_socket socket, sptr_byte_stream stream )
{
	Deserialize( stream );

	auto user = UserManager::Get().FindUserBySocket( socket );

	if( user == nullptr )
	{
		Log::Error( "User not found! [{}]", m_sessionId );
		return std::make_shared< ResultUpdateGameData >( this );
	}

	auto gameSession = GameSessionManager::Get().FindGame( user->m_gameId, user->m_gameType );

	if( !ParseGameData( gameSession ) )
	{
		Log::Error( "Failed to parse game data! [{}]", m_sessionId );
		return std::make_shared< ResultUpdateGameData >( this );
	}

	const auto localAddr = std::string( m_gameData.c_str() + 220, 24 );
	user->m_localAddr = localAddr;

	return std::make_shared< ResultUpdateGameData >( this );
}

ResultUpdateGameData::ResultUpdateGameData( GenericRequest *request ) : GenericResponse( *request )
{

}

void ResultUpdateGameData::Serialize( ByteBuffer &out ) const
{
	out.write_u16( m_packetId );
	out.write_u32( m_trackId );
	out.write_u32( 0 );
}
