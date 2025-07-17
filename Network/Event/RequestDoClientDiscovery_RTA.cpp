#include "RequestDoClientDiscovery_RTA.h"

#include "../../Game/RealmUserManager.h"
#include "../../Game/GameSessionManager.h"
#include "../../configuration.h"
#include "../../logging.h"

void RequestDoClientDiscovery_RTA::Deserialize( sptr_byte_stream stream )
{
	DeserializeHeader( stream );

	m_sessionId = stream->read_encrypted_utf16();
	m_gameId = stream->read_u32();
	m_localAddr = stream->read_utf16();
	m_localPort = stream->read_u32();
}

sptr_generic_response RequestDoClientDiscovery_RTA::ProcessRequest( sptr_socket socket, sptr_byte_stream stream )
{
	Deserialize( stream );

	auto user = UserManager::Get().FindUserBySocket( socket );
	if( user == nullptr )
	{
		Log::Error( "User not found! [{}]", m_sessionId );
		return std::make_shared< ResultDoClientDiscovery_RTA >( this, TIMEOUT );
	}

	auto session = GameSessionManager::Get().FindGame( m_gameId, user->m_gameType );
	if( session == nullptr )
	{
		Log::Error( "Game session not found! [{}]", m_gameId );
		return std::make_shared< ResultDoClientDiscovery_RTA >( this, NOT_FOUND );
	}

	user->m_isHost = false;
	user->m_gameId = session->m_gameId;
	user->m_localAddr = Util::WideToUTF8(m_localAddr);
	user->m_localPort = m_localPort;

	return std::make_shared< ResultDoClientDiscovery_RTA >( this, SUCCESS, Config::service_ip, Config::discovery_port );
}

ResultDoClientDiscovery_RTA::ResultDoClientDiscovery_RTA( GenericRequest *request, int32_t reply, std::string discoveryIp, int32_t discoveryPort ) : GenericResponse( *request )
{
	m_reply = reply;
	m_discoveryIp = discoveryIp;
	m_discoveryPort = discoveryPort;
}

void ResultDoClientDiscovery_RTA::Serialize( ByteBuffer &out ) const
{
	out.write_u16( m_packetId );
	out.write_u32( m_trackId );
	out.write_u32( m_reply );

	out.write_utf8( m_discoveryIp );
	out.write_u32( m_discoveryPort );
}
