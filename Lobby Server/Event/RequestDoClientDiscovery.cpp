#include "../../global_define.h"
#include "RequestDoClientDiscovery.h"
#include "NotifyClientReqConnect.h"

void RequestDoClientDiscovery::Deserialize( sptr_tcp_socket socket, sptr_byte_stream stream )
{
	DeserializeHeader( stream );

	m_sessionId = stream->read_encrypted_utf16();
	m_gameId = stream->read_u32();
}

sptr_generic_response RequestDoClientDiscovery::ProcessRequest( sptr_tcp_socket socket, sptr_byte_stream stream )
{
	Deserialize( socket, stream );

	auto user = RealmUserManager::Get().GetUser( socket );

	if( user == nullptr )
	{
		Log::Error( "User not found! [%s]", m_sessionId.c_str() );
		return std::make_shared< ResultDoClientDiscovery >( this, DISCOVERY_REPLY::FATAL_ERROR, "", 0 );
	}

	if( user->m_sessionId != m_sessionId )
	{
		Log::Error( "Session ID mismatch! [%s]", m_sessionId.c_str() );
		return std::make_shared< ResultDoClientDiscovery >( this, DISCOVERY_REPLY::FATAL_ERROR, "", 0 );
	}

	auto result = GameSessionManager::Get().UserJoinGame( m_gameId, user );

	if( result == false )
	{
		Log::Error( "Failed to join game! [%d]", m_gameId );
		return std::make_shared< ResultDoClientDiscovery >( this, DISCOVERY_REPLY::GAME_FULL, "", 0 );
	}

	return std::make_shared< ResultDoClientDiscovery >( this, DISCOVERY_REPLY::SUCCESS, "192.168.1.248", 40820 );
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
