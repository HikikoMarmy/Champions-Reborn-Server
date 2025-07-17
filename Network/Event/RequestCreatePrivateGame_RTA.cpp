#include "RequestCreatePrivateGame_RTA.h"

#include "../../Game/RealmUserManager.h"
#include "../../Game/GameSessionManager.h"
#include "../../configuration.h"
#include "../../logging.h"

void RequestCreatePrivateGame_RTA::Deserialize( sptr_byte_stream stream )
{
	DeserializeHeader( stream );

	m_sessionId = stream->read_encrypted_utf16();
	m_gameName = stream->read_utf16();
	m_localAddr = Util::WideToUTF8( stream->read_utf16() );
	m_localPort = stream->read_u32();
}

sptr_generic_response RequestCreatePrivateGame_RTA::ProcessRequest( sptr_socket socket, sptr_byte_stream stream )
{
	Deserialize( stream );

	auto user = UserManager::Get().FindUserBySocket( socket );
	if( user == nullptr )
	{
		Log::Error( "User not found! [{}]", m_sessionId );
		return std::make_shared< ResultCreatePrivateGame_RTA >( this, FATAL_ERROR, "", 0 );
	}

	auto result = GameSessionManager::Get().CreateGameSession_RTA( user, L"", m_gameName, {}, true);
	if( !result )
	{
		Log::Error( "RequestCreatePrivateGame2::ProcessRequest() - Failed to create private game session!" );
		return std::make_shared< ResultCreatePrivateGame_RTA >( this, GENERAL_ERROR, "", 0 );
	}

	user->m_localAddr = m_localAddr;
	user->m_localPort = m_localPort;

	Log::Info( "[{}] Create Private Game: {}", m_sessionId, m_gameName );

	return std::make_shared< ResultCreatePrivateGame_RTA >( this, SUCCESS, Config::service_ip, Config::discovery_port );
}

ResultCreatePrivateGame_RTA::ResultCreatePrivateGame_RTA( GenericRequest *request, int32_t reply, std::string discoveryIp, int32_t discoveryPort ) : GenericResponse( *request )
{
	m_reply = reply;
	m_discoveryIp = discoveryIp;
	m_discoveryPort = discoveryPort;
}

void ResultCreatePrivateGame_RTA::Serialize( ByteBuffer &out ) const
{
	out.write_u16( m_packetId );
	out.write_u32( m_trackId );
	out.write_u32( m_reply );

	out.write_utf8( m_discoveryIp );
	out.write( m_discoveryPort );
}
