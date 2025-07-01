#include "RequestCreatePublicGame.h"

#include "../../Game/RealmUserManager.h"
#include "../../Game/GameSessionManager.h"
#include "../../configuration.h"
#include "../../logging.h"

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

	auto [name, stage] = ParseNameAndStage( m_gameInfo );

	m_gameName = name;
	m_stageName = stage;
}

std::tuple<std::wstring, std::wstring> RequestCreatePublicGame::ParseNameAndStage( const std::wstring &gameInfo )
{
	const size_t open = gameInfo.find( L'[' );
	const size_t close = gameInfo.find( L']' );

	if( open == std::wstring::npos || close == std::wstring::npos || close < open )
		return { L"", L"" };

	std::wstring name = gameInfo.substr( 0, open );
	std::wstring stage = gameInfo.substr( open + 1, close - open - 1 );

	if( !name.empty() && iswspace( name.back() ) )
		name.pop_back();

	return { name, stage };
}

sptr_generic_response RequestCreatePublicGame::ProcessRequest( sptr_socket socket, sptr_byte_stream stream )
{
	Deserialize( stream );

	Log::Packet( stream->get_buffer(), stream->get_length(), false );

	auto user = UserManager::Get().FindUserBySocket( socket );
	if( user == nullptr )
	{
		Log::Error( "User not found! [{}]", m_sessionId );
		return std::make_shared< ResultCreatePublicGame >( this, FATAL_ERROR, "", 0 );
	}

	if( nullptr != GameSessionManager::Get().FindGame( m_gameName, user->m_gameType ) )
	{
		Log::Error( "Game name is already in use! [{}]", m_gameName );
		return std::make_shared< ResultCreatePublicGame >( this, GAME_NAME_IN_USE, "", 0 );
	}

	auto result = GameSessionManager::Get().CreateGameSession_CON( user, m_gameInfo, m_gameName, m_stageName, false );

	if( !result )
	{
		Log::Error( "RequestCreatePublicGame::ProcessRequest() - Failed to create public game session!" );
		return std::make_shared< ResultCreatePublicGame >( this, GENERAL_ERROR, "", 0 );
	}

	Log::Info( "[{}] Create Public Game: {}", m_sessionId, m_gameInfo );

	user->m_isHost = true;
	user->m_discoveryAddr = "";
	user->m_discoveryPort = 0;

	return std::make_shared< ResultCreatePublicGame >( this, SUCCESS, Config::service_ip, Config::discovery_port );
}

// Result
ResultCreatePublicGame::ResultCreatePublicGame( GenericRequest *request, int32_t reply, std::string discoveryIp, int32_t discoveryPort ) : GenericResponse( *request )
{
	m_reply = reply;
	m_discoveryIp = discoveryIp;
	m_discoveryPort = discoveryPort;
}

void ResultCreatePublicGame::Serialize( ByteBuffer &out ) const
{
	out.write_u16( m_packetId );
	out.write_u32( m_trackId );
	out.write_u32( m_reply );

	out.write_sz_utf8( m_discoveryIp );
	out.write( m_discoveryPort );
}
