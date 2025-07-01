#include "RequestDoClientDiscovery.h"

#include "../../Game/RealmUserManager.h"
#include "../../Game/GameSessionManager.h"
#include "../../configuration.h"
#include "../../logging.h"

void RequestDoClientDiscovery::Deserialize( sptr_byte_stream stream )
{
	DeserializeHeader( stream );

	m_sessionId = stream->read_encrypted_utf16();
	m_gameId = stream->read_u32();
}

sptr_generic_response RequestDoClientDiscovery::ProcessRequest( sptr_socket socket, sptr_byte_stream stream )
{
	Deserialize( stream );

	Log::Packet( stream->get_buffer(), stream->get_length(), false );	

	auto user = UserManager::Get().FindUserBySocket( socket );
	if( user == nullptr )
	{
		return std::make_shared< ResultDoClientDiscovery >( this, DISCOVERY_REPLY::FATAL_ERROR, "", 0 );
	}

	auto session = GameSessionManager::Get().FindGame( m_gameId, user->m_gameType );
	if( session == nullptr )
	{
		return std::make_shared< ResultDoClientDiscovery >( this, DISCOVERY_REPLY::FATAL_ERROR, "", 0 );
	}

	if( !session->IsJoinable( user ) )
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

void ResultDoClientDiscovery::Serialize( ByteBuffer &out ) const
{
	out.write_u16( m_packetId );
	out.write_u32( m_trackId );
	out.write_u32( m_reply );

	out.write_sz_utf8( m_discoveryIP );
	out.write( m_discoveryPort );
}
