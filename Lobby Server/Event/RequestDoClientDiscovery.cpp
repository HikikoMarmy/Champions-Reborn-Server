#include "../../global_define.h"
#include "RequestDoClientDiscovery.h"
#include "NotifyClientDiscovered.h"
#include "NotifyGameDiscovered.h"
#include "NotifyClientReqConnect.h"

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
		Log::Error( "Game session not found! [%d]", m_gameId );
		return std::make_shared< ResultDoClientDiscovery >( this, DISCOVERY_REPLY::FATAL_ERROR, "", 0 );
	}

	// TODO: Get max size from game data blob
	if( session->m_userList.size() >= 4 )
	{
		Log::Error( "Game session is full! [%d]", m_gameId );
		return std::make_shared< ResultDoClientDiscovery >( this, DISCOVERY_REPLY::GAME_FULL, "", 0 );
	}

	auto host = session->GetHost();

	if( host == nullptr )
	{
		Log::Error( "Game session owner not found! [%d]", m_gameId );
		return std::make_shared< ResultDoClientDiscovery >( this, DISCOVERY_REPLY::FATAL_ERROR, "", 0 );
	}

	Log::Debug( "[%s] DoClientDiscovery : %d", m_sessionId.c_str(), m_gameId );

	user->game_id = m_gameId;
	user->discovery_state = DiscoveryState::Initial_Connect;

	// Send the discovery server information to the client
	return std::make_shared< ResultDoClientDiscovery >(this, DISCOVERY_REPLY::SUCCESS, Config::service_ip, Config::discovery_port );
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
