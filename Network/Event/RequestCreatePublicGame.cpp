#include "../../global_define.h"

#include "RequestCreatePublicGame.h"

// Request
void RequestCreatePublicGame::Deserialize( sptr_byte_stream stream )
{
	DeserializeHeader( stream );

	m_sessionId = stream->read_encrypted_utf16();

	// Some kind of match attributes
	auto unknown_a = stream->read_u16();
	auto unknown_b = stream->read_u32();
	auto unknown_c = stream->read_u32();
	auto unknown_d = stream->read_u32();

	m_gameInfo = stream->read_utf16();
}

sptr_generic_response RequestCreatePublicGame::ProcessRequest( sptr_user user, sptr_byte_stream stream )
{
	Deserialize( stream );

	auto result = GameSessionManager::Get().CreatePublicGameSession( user, m_gameInfo, user->m_clientType );

	if( !result )
	{
		Log::Error( "RequestCreatePublicGame::ProcessRequest() - Failed to create public game session!" );
		return std::make_shared< ResultCreatePublicGame >( this, CREATE_REPLY::GENERAL_ERROR, "", 0 );
	}

	Log::Info( "[%S] Create Public Game: %S", m_sessionId.c_str(), m_gameInfo.c_str() );

	return std::make_shared< ResultCreatePublicGame >(this, CREATE_REPLY::SUCCESS, Config::service_ip, Config::discovery_port);
}

// Result
ResultCreatePublicGame::ResultCreatePublicGame( GenericRequest *request, int32_t reply, std::string discoveryIp, int32_t discoveryPort ) : GenericResponse( *request )
{
	m_reply = reply;
	m_discoveryIp = discoveryIp;
	m_discoveryPort = discoveryPort;
}

ByteStream &ResultCreatePublicGame::Serialize()
{
	m_stream.write_u16( m_packetId );
	m_stream.write_u32( m_requestId );
	m_stream.write_u32( m_reply );

	m_stream.write_sz_utf8( m_discoveryIp );
	m_stream.write( m_discoveryPort );

	return m_stream;
}
