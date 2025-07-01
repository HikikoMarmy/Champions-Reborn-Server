#include "RequestCreatePrivateGame.h"

#include "../../Game/RealmUserManager.h"
#include "../../Game/GameSessionManager.h"
#include "../../configuration.h"
#include "../../logging.h"

void RequestCreatePrivateGame::Deserialize( sptr_byte_stream stream )
{
	DeserializeHeader( stream );

	m_sessionId = stream->read_encrypted_utf16();
	m_gameName = stream->read_utf16();
}

sptr_generic_response RequestCreatePrivateGame::ProcessRequest( sptr_socket socket, sptr_byte_stream stream )
{
	Deserialize( stream );

	auto user = UserManager::Get().FindUserBySocket( socket );
	if( user == nullptr )
	{
		Log::Error( "User not found! [{}]", m_sessionId );
		return std::make_shared< ResultCreatePrivateGame >( this, FATAL_ERROR, "", 0 );
	}

	if( nullptr != GameSessionManager::Get().FindGame( m_gameName, user->m_gameType ) )
	{
		Log::Error( "Game name is already in use! [{}]", m_gameName );
		return std::make_shared< ResultCreatePrivateGame >( this, GAME_NAME_IN_USE, "", 0 );
	}

	auto result = GameSessionManager::Get().CreateGameSession_CON( user, L"", m_gameName, L"", true);

	if( !result )
	{
		Log::Error( "RequestCreatePrivateGame::ProcessRequest() - Failed to create private game session!" );
		return std::make_shared< ResultCreatePrivateGame >( this, GENERAL_ERROR, "", 0 );
	}

	Log::Info( "[{}] Create Private Game: {}", m_sessionId, m_gameName );

	return std::make_shared< ResultCreatePrivateGame >( this, SUCCESS, Config::service_ip, Config::discovery_port );
}

ResultCreatePrivateGame::ResultCreatePrivateGame( GenericRequest *request, int32_t reply, std::string discoveryIp, int32_t discoveryPort ) : GenericResponse( *request )
{
	m_reply = reply;
	m_discoveryIp = discoveryIp;
	m_discoveryPort = discoveryPort;
}

void ResultCreatePrivateGame::Serialize( ByteBuffer &out ) const
{
	out.write_u16( m_packetId );
	out.write_u32( m_trackId );
	out.write_u32( m_reply );

	out.write_sz_utf8( m_discoveryIp );
	out.write( m_discoveryPort );
}
