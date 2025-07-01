#include "RequestGetGame.h"

#include "../../Game/RealmUserManager.h"
#include "../../Game/GameSessionManager.h"
#include "../../Game/RealmUser.h"
#include "../../logging.h"

void RequestGetGame::Deserialize( sptr_byte_stream stream )
{
	DeserializeHeader( stream );

	m_sessionId = stream->read_encrypted_utf16();
	m_gameName = stream->read_utf16();
}

sptr_generic_response RequestGetGame::ProcessRequest( sptr_socket socket, sptr_byte_stream stream )
{
	Deserialize( stream );

	auto user = UserManager::Get().FindUserBySocket( socket );
	if( user == nullptr )
	{
		Log::Error( "User not found! [{}]", m_sessionId );
		return std::make_shared< ResultGetGame >( this, TIMEOUT );
	}

	auto session = GameSessionManager::Get().FindGame( m_gameName, user->m_gameType );

	if( session == nullptr )
	{
		Log::Error( "Game session not found! [{}]", m_gameName );
		return std::make_shared< ResultGetGame >( this, NOT_FOUND );
	}

	if( session->m_currentPlayers >= session->m_maximumPlayers )
	{
		Log::Error( "Game session is full! [{}]", m_gameName );
		return std::make_shared< ResultGetGame >( this, TIMEOUT );
	}

	auto host_user = session->GetOwner();

	if( host_user == nullptr )
	{
		Log::Error( "Game session owner not found! [{}]", m_gameName );
		return std::make_shared< ResultGetGame >( this, TIMEOUT );
	}

	user->m_isHost = false;
	user->m_gameId = session->m_gameId;

	return std::make_shared< ResultGetGame >( this, SUCCESS, session->m_gameId );
}

ResultGetGame::ResultGetGame( GenericRequest *request, int32_t reply, int32_t gameId ) : GenericResponse( *request )
{
	m_reply = reply;
	m_gameId = gameId;
}

void ResultGetGame::Serialize( ByteBuffer &out ) const
{
	out.write_u16( m_packetId );
	out.write_u32( m_trackId );
	out.write_u32( m_reply );

	// TODO: These may come in from the UpdateGameData event
	out.write_utf16( L"Kelethin" );
	out.write_utf16( L"OwnerName" );

	out.write_u32( m_gameId );
}
