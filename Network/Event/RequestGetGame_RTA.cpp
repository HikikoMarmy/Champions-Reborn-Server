#include "RequestGetGame_RTA.h"

#include "../../Game/RealmUserManager.h"
#include "../../Game/GameSessionManager.h"
#include "../../Game/RealmUser.h"
#include "../../logging.h"

void RequestGetGame_RTA::Deserialize( sptr_byte_stream stream )
{
	DeserializeHeader( stream );

	m_sessionId = stream->read_encrypted_utf16();
	m_gameName = stream->read_utf16();
}

sptr_generic_response RequestGetGame_RTA::ProcessRequest( sptr_socket socket, sptr_byte_stream stream )
{
	Log::Packet( stream->get_buffer(), stream->get_length(), false );

	Deserialize( stream );

	auto user = UserManager::Get().FindUserBySocket( socket );
	if( user == nullptr )
	{
		Log::Error( "User not found! [{}]", m_sessionId );
		return std::make_shared< ResultGetGame_RTA >( this, TIMEOUT );
	}

	auto session = GameSessionManager::Get().FindGame( m_gameName, user->m_gameType );

	if( session == nullptr )
	{
		Log::Error( "Game session not found! [{}]", m_gameName );
		return std::make_shared< ResultGetGame_RTA >( this, NOT_FOUND );
	}

	if( session->m_currentPlayers >= session->m_maximumPlayers )
	{
		Log::Error( "Game session is full! [{}]", m_gameName );
		return std::make_shared< ResultGetGame_RTA >( this, TIMEOUT );
	}

	auto host_user = session->GetOwner();

	if( host_user == nullptr )
	{
		Log::Error( "Game session owner not found! [{}]", m_gameName );
		return std::make_shared< ResultGetGame_RTA >( this, TIMEOUT );
	}

	user->m_isHost = false;
	user->m_gameId = session->m_gameId;

	return std::make_shared< ResultGetGame_RTA >( this, SUCCESS, session->m_gameId, host_user->m_discoveryAddr, host_user->m_localAddr, host_user->m_discoveryPort );
}

ResultGetGame_RTA::ResultGetGame_RTA( GenericRequest *request, int32_t reply, int32_t gameId, std::string discoveryAddr, std::string localAddr, int32_t discoveryPort ) : GenericResponse( *request )
{
	m_reply = reply;
	m_gameId = gameId;
	m_discoveryAddr = Util::UTF8ToWide( discoveryAddr );
	m_localAddr = Util::UTF8ToWide( localAddr );
	m_port = discoveryPort;
}

void ResultGetGame_RTA::Serialize( ByteBuffer &out ) const
{
	out.write_u16( m_packetId );
	out.write_u32( m_trackId );
	out.write_u32( m_reply );

	out.write_utf16( L"Test" );
	out.write_u32( m_gameId );

	out.write_utf16( m_discoveryAddr );
	out.write_u32( m_port );

	out.write_utf16( m_localAddr );
	out.write_u32( m_port );
}
