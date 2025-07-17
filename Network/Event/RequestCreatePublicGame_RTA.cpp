#include "RequestCreatePublicGame_RTA.h"

#include "../../Game/RealmUserManager.h"
#include "../../Game/GameSessionManager.h"
#include "../../Game/ChatRoomManager.h"
#include "../../configuration.h"
#include "../../logging.h"

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

	m_localAddr = Util::WideToUTF8( stream->read_utf16() );
	m_localPort = stream->read_u32();

	if( !ParseGameInfo() )
	{
		Log::Error( "Failed to parse game info: {}", m_gameInfo );
	}
}

bool RequestCreatePublicGame_RTA::ParseGameInfo()
{
	if( m_gameInfo.empty() )
	{
		Log::Error( "Game info is empty!" );
		return false;
	}

	size_t pipePos = m_gameInfo.find( L'|' );
	if( pipePos == std::wstring::npos )
	{
		Log::Error( "Invalid game info format!" );
		return false;
	}

	std::wstring numbersPart = m_gameInfo.substr( 0, pipePos );
	m_gameName = m_gameInfo.substr( pipePos + 1 );
	std::wstringstream ss( numbersPart );
	std::wstring numStr;
	std::array<int8_t, 5> fields = { 0, 0, 0, 0, 0 };

	for( size_t i = 0; i < fields.size(); ++i )
	{
		if( !std::getline( ss, numStr, L',' ) )
			return false;
		try
		{
			fields[ i ] = std::stoi( numStr );
		}
		catch( ... )
		{
			return false;
		}
	}

	return true;
}

sptr_generic_response RequestCreatePublicGame_RTA::ProcessRequest( sptr_socket socket, sptr_byte_stream stream )
{
	Deserialize( stream );

	auto user = UserManager::Get().FindUserBySocket( socket );
	if( user == nullptr )
	{
		Log::Error( "User not found! [{}]", m_sessionId );
		return std::make_shared< ResultCreatePublicGame_RTA >( this, GENERAL_ERROR, "", 0 );
	}

	auto result = GameSessionManager::Get().CreateGameSession_RTA( user, m_gameInfo, m_gameName, m_attributes, false );
	if( !result )
	{
		Log::Error( "RequestCreatePublicGame::ProcessRequest() - Failed to create public game session!" );
		return std::make_shared< ResultCreatePublicGame_RTA >( this, GENERAL_ERROR, "", 0 );
	}

	user->m_localAddr = m_localAddr;
	user->m_localPort = m_localPort;

	Log::Info( "[{}] Create Public Game: {}", user->m_username, m_gameInfo );

	return std::make_shared< ResultCreatePublicGame_RTA >( this, SUCCESS, Config::service_ip, Config::discovery_port );
}

// Result
ResultCreatePublicGame_RTA::ResultCreatePublicGame_RTA( GenericRequest *request, int32_t reply, std::string discoveryIp, int32_t discoveryPort ) : GenericResponse( *request )
{
	m_reply = reply;
	m_discoveryIp = discoveryIp;
	m_discoveryPort = discoveryPort;
}

void ResultCreatePublicGame_RTA::Serialize( ByteBuffer &out ) const
{
	out.write_u16( m_packetId );
	out.write_u32( m_trackId );
	out.write_u32( m_reply );

	out.write_utf8( m_discoveryIp );
	out.write( m_discoveryPort );
}
