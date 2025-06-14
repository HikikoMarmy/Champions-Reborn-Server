#include "RequestStartGame.h"

#include "../../Game/RealmUserManager.h"
#include "../../Game/GameSessionManager.h"
#include "../../Game/RealmUser.h"
#include "../../logging.h"

void RequestStartGame::Deserialize( sptr_byte_stream stream )
{
	DeserializeHeader( stream );

	auto sessionId = stream->read_encrypted_utf16();
}

sptr_generic_response RequestStartGame::ProcessRequest( sptr_socket socket, sptr_byte_stream stream )
{
	Deserialize( stream );

	auto user = RealmUserManager::Get().FindUserBySocket( socket );
	if( user == nullptr )
	{
		return std::make_shared< ResultStartGame >( this, FATAL_ERROR );
	}

	auto session = GameSessionManager::Get().FindGame( user->m_gameId, user->m_gameType );
	if( session == nullptr )
	{
		Log::Error( "Game session not found! [%d]", user->m_gameId );
		return std::make_shared< ResultStartGame >( this, FATAL_ERROR );
	}

	GameSessionManager::Get().RequestStart(user);

	return std::make_shared< ResultStartGame >( this, SUCCESS );
}

ResultStartGame::ResultStartGame( GenericRequest *request, int32_t reply ) : GenericResponse( *request )
{
	m_reply = reply;
}

ByteBuffer& ResultStartGame::Serialize()
{
	m_stream.write_u16( m_packetId );
	m_stream.write_u32( m_trackId );
	m_stream.write_u32( m_reply );

	return m_stream;
}
