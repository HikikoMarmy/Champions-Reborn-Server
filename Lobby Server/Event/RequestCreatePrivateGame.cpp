#include "../../global_define.h"

#include "RequestCreatePrivateGame.h"

void RequestCreatePrivateGame::Deserialize( sptr_byte_stream stream )
{
	DeserializeHeader( stream );

	m_sessionId = stream->read_encrypted_utf16();
	m_gameName = stream->read_utf16();
}

sptr_generic_response RequestCreatePrivateGame::ProcessRequest( sptr_user user, sptr_byte_stream stream )
{
	Deserialize( stream );

	auto result = GameSessionManager::Get().CreatePrivateGameSession( user, m_gameName, 0, 9999 );

	if( !result )
	{
		Log::Error( "RequestCreatePrivateGame::ProcessRequest() - Failed to create private game session!" );
		return std::make_shared< ResultCreatePrivateGame >( this, CREATE_REPLY::GENERAL_ERROR, "", 0 );
	}

	Log::Info( "[%S] Create Private Game: %S", m_sessionId.c_str(), m_gameName.c_str() );

	return std::make_shared< ResultCreatePrivateGame >( this, CREATE_REPLY::SUCCESS, Config::service_ip, Config::discovery_port );
}

ResultCreatePrivateGame::ResultCreatePrivateGame( GenericRequest *request, int32_t reply, std::string discoveryIp, int32_t discoveryPort ) : GenericResponse( *request )
{
	m_reply = reply;
	m_discoveryIp = discoveryIp;
	m_discoveryPort = discoveryPort;
}

ByteStream &ResultCreatePrivateGame::Serialize()
{
	m_stream.write_u16( m_packetId );
	m_stream.write_u32( m_requestId );
	m_stream.write_u32( m_reply );

	m_stream.write_sz_utf8( m_discoveryIp );
	m_stream.write( m_discoveryPort );

	return m_stream;
}
