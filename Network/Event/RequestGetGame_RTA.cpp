#include "RequestGetGame_RTA.h"

#include "../../Game/RealmUserManager.h"
#include "../../Game/GameSessionManager.h"
#include "../../configuration.h"
#include "../../logging.h"

void RequestGetGame_RTA::Deserialize( sptr_byte_stream stream )
{
	DeserializeHeader( stream );

	m_sessionId = stream->read_encrypted_utf16();
	m_gameId = stream->read_u32();
	m_localAddr = Util::WideToUTF8(stream->read_utf16());
}

sptr_generic_response RequestGetGame_RTA::ProcessRequest( sptr_socket socket, sptr_byte_stream stream )
{
	Deserialize( stream );

	auto user = RealmUserManager::Get().FindUserBySocket( socket );
	if( user == nullptr )
	{
		Log::Error( "User not found! [%S]", m_sessionId.c_str() );
		return std::make_shared< ResultGetGame_RTA >( this, TIMEOUT );
	}

	auto session = GameSessionManager::Get().FindGame( m_gameId, user->m_gameType );

	if( session == nullptr )
	{
		Log::Error( "Game session not found! [%d]", m_gameId );
		return std::make_shared< ResultGetGame_RTA >( this, NOT_FOUND );
	}

	if( session->m_currentPlayers >= session->m_maximumPlayers )
	{
		Log::Error( "Game session is full! [%d]", m_gameId );
		return std::make_shared< ResultGetGame_RTA >( this, TIMEOUT );
	}

	auto host_user = session->m_owner.lock();

	if( host_user == nullptr )
	{
		Log::Error( "Game session owner not found! [%d]", m_gameId );
		return std::make_shared< ResultGetGame_RTA >( this, TIMEOUT );
	}

	user->m_isHost = false;
	user->m_gameId = session->m_gameIndex;
	user->m_localAddr = m_localAddr;

	return std::make_shared< ResultGetGame_RTA >( this, SUCCESS, Config::service_ip, Config::discovery_port );
}

ResultGetGame_RTA::ResultGetGame_RTA( GenericRequest *request, int32_t reply, std::string discoveryIp, int32_t discoveryPort ) : GenericResponse( *request )
{
	m_reply = reply;
	m_discoveryIp = discoveryIp;
	m_discoveryPort = discoveryPort;
}

ByteBuffer &ResultGetGame_RTA::Serialize()
{
	m_stream.write_u16( m_packetId );
	m_stream.write_u32( m_trackId );
	m_stream.write_u32( m_reply );

	m_stream.write_utf8( m_discoveryIp );
	m_stream.write_u32( m_discoveryPort );

	return m_stream;
}
