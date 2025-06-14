#include "RequestCreatePrivateGame_RTA.h"

#include "../../Game/RealmUserManager.h"
#include "../../Game/GameSessionManager.h"
#include "../../configuration.h"
#include "../../logging.h"

void RequestCreatePrivateGame_RTA::Deserialize( sptr_byte_stream stream )
{
	DeserializeHeader( stream );

	m_sessionId = stream->read_encrypted_utf16();
	m_gameInfo = stream->read_utf16();
}

sptr_generic_response RequestCreatePrivateGame_RTA::ProcessRequest( sptr_socket socket, sptr_byte_stream stream )
{
	Deserialize( stream );

	auto user = RealmUserManager::Get().FindUserBySocket( socket );
	if( user == nullptr )
	{
		Log::Error( "User not found! [%S]", m_sessionId.c_str() );
		return std::make_shared< ResultCreatePrivateGame2 >( this, CREATE_REPLY::FATAL_ERROR, "", 0 );
	}

	auto result = GameSessionManager::Get().CreatePrivateGameSession( user, m_gameInfo, user->m_gameType );

	if( !result )
	{
		Log::Error( "RequestCreatePrivateGame2::ProcessRequest() - Failed to create private game session!" );
		return std::make_shared< ResultCreatePrivateGame2 >( this, CREATE_REPLY::GENERAL_ERROR, "", 0 );
	}

	Log::Info( "[%S] Create Private Game: %S", m_sessionId.c_str(), m_gameInfo.c_str() );

	return std::make_shared< ResultCreatePrivateGame2 >( this, CREATE_REPLY::SUCCESS, Config::service_ip, Config::discovery_port );
}

ResultCreatePrivateGame2::ResultCreatePrivateGame2( GenericRequest *request, int32_t reply, std::string discoveryIp, int32_t discoveryPort ) : GenericResponse( *request )
{
	m_reply = reply;
	m_discoveryIp = discoveryIp;
	m_discoveryPort = discoveryPort;
}

ByteBuffer &ResultCreatePrivateGame2::Serialize()
{
	m_stream.write_u16( m_packetId );
	m_stream.write_u32( m_trackId );
	m_stream.write_u32( m_reply );

	m_stream.write_sz_utf8( m_discoveryIp );
	m_stream.write( m_discoveryPort );

	return m_stream;
}
