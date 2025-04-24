#include "../../global_define.h"
#include "RequestDoClientDiscovery.h"

void RequestDoClientDiscovery::Deserialize( sptr_byte_stream stream )
{
	DeserializeHeader( stream );

	m_sessionId = stream->read_encrypted_utf16();
	m_gameId = stream->read_u32();
}

sptr_generic_response RequestDoClientDiscovery::ProcessRequest( sptr_user user, sptr_byte_stream stream )
{
	Deserialize( stream );

	auto session = GameSessionManager::Get().FindGame( m_gameId );
	if( session == nullptr )
	{
		return std::make_shared< ResultDoClientDiscovery >( this, DISCOVERY_REPLY::FATAL_ERROR, "", 0 );
	}

	if( session->m_currentPlayers >= session->m_maximumPlayers )
	{
		return std::make_shared< ResultDoClientDiscovery >( this, DISCOVERY_REPLY::GAME_FULL, "", 0 );
	}

	user->m_gameId = m_gameId;
	user->m_isHost = false;

	return std::make_shared< ResultDoClientDiscovery >( this, DISCOVERY_REPLY::SUCCESS, Config::service_ip, Config::discovery_port );
}

ResultDoClientDiscovery::ResultDoClientDiscovery( GenericRequest *request, int32_t reply, std::string ip, int32_t port ) : GenericResponse( *request )
{
	m_reply = reply;
	m_discoveryIP = ip;
	m_discoveryPort = port;
}

ByteStream &ResultDoClientDiscovery::Serialize()
{
	m_stream.write_u16( m_packetId );
	m_stream.write_u32( m_requestId );
	m_stream.write_u32( m_reply );

	m_stream.write_sz_utf8( m_discoveryIP );
	m_stream.write( m_discoveryPort );

	return m_stream;
}
