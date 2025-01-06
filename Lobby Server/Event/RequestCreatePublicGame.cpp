#include "../../global_define.h"

#include "RequestCreatePublicGame.h"

void RequestCreatePublicGame::Deserialize( sptr_tcp_socket socket, sptr_byte_stream stream )
{
	DeserializeHeader( stream );

	m_sessionId = stream->read_encrypted_utf16();

	// Some kind of match attributes
	auto unknown_a = stream->read_u16();
	auto unknown_b = stream->read_u32();
	auto unknown_c = stream->read_u32();
	auto unknown_d = stream->read_u32();

	m_gameName = stream->read_utf16();
}

sptr_generic_response RequestCreatePublicGame::ProcessRequest( sptr_tcp_socket socket, sptr_byte_stream stream )
{
	Deserialize( socket, stream );

	auto user = RealmUserManager::Get().GetUser( socket );

	if( user == nullptr )
	{
		Log::Error( "User not found! [%S]", m_sessionId.c_str() );
		return std::make_shared< ResultCreatePublicGame >( this, CREATE_REPLY::FATAL_ERROR, "", 0 );
	}

	auto &game_manager = GameSessionManager::Get();

	auto result = game_manager.CreatePublicGameSession( user, m_gameName, 0, 9999 );

	if( !result )
	{
		Log::Error( "RequestCreatePublicGame::ProcessRequest() - Failed to create public game session!" );
		return std::make_shared< ResultCreatePublicGame >( this, CREATE_REPLY::GENERAL_ERROR, "", 0 );
	}

	Log::Info( "[%S] Create Public Game: %S", m_sessionId.c_str(), m_gameName.c_str() );

	user->m_state = RealmUser::UserState::InGameLobby;

	// Send the discovery server information to the client
	return std::make_shared< ResultCreatePublicGame >( this, CREATE_REPLY::SUCCESS, "192.168.1.248", 3008 );
}

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
