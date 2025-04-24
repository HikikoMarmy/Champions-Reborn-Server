#include "../../global_define.h"

#include "RequestCreatePublicGame_RTA.h"

// Request
void RequestCreatePublicGame_RTA::Deserialize( sptr_byte_stream stream )
{
	DeserializeHeader( stream );

	m_sessionId = stream->read_encrypted_utf16();

	auto unknown_a = stream->read_u16();
	auto unknown_b = stream->read_u32();
	auto unknown_c = stream->read_u32();
	auto unknown_d = stream->read_u32();

	m_gameInfo = stream->read_utf16();

	auto unknown_e = stream->read_u32();
	auto unknown_f = stream->read_u32();

	m_localAddr = stream->read_utf16();
}

sptr_generic_response RequestCreatePublicGame_RTA::ProcessRequest( sptr_user user, sptr_byte_stream stream )
{
	Deserialize( stream );

	Log::Packet( stream->data, stream->data.size(), false );

	auto result = GameSessionManager::Get().CreatePublicGameSession( user, m_gameInfo, user->m_clientType );

	if( !result )
	{
		Log::Error( "RequestCreatePublicGame::ProcessRequest() - Failed to create public game session!" );
		return std::make_shared< ResultCreatePublicGame_RTA >( this, CREATE_REPLY::GENERAL_ERROR, "", 0 );
	}

	user->m_localAddr = Utility::WideToUTF8( m_localAddr );

	Log::Info( "[%S] Create Public Game: %S", m_sessionId.c_str(), m_gameInfo.c_str() );

	return std::make_shared< ResultCreatePublicGame_RTA >( this, CREATE_REPLY::SUCCESS, Config::service_ip, Config::discovery_port );
}

// Result
ResultCreatePublicGame_RTA::ResultCreatePublicGame_RTA( GenericRequest *request, int32_t reply, std::string discoveryIp, int32_t discoveryPort ) : GenericResponse( *request )
{
	m_reply = reply;
	m_discoveryIp = discoveryIp;
	m_discoveryPort = discoveryPort;
}

ByteStream &ResultCreatePublicGame_RTA::Serialize()
{
	m_stream.write_u16( m_packetId );
	m_stream.write_u32( m_requestId );
	m_stream.write_u32( m_reply );

	m_stream.write_utf8( m_discoveryIp );
	m_stream.write( m_discoveryPort );

	return m_stream;
}
