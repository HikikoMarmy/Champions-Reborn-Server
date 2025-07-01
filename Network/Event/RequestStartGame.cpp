#include "RequestStartGame.h"

#include "../../Game/RealmUserManager.h"
#include "../../Game/GameSessionManager.h"
#include "../../Game/ChatRoomManager.h"
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

	auto user = UserManager::Get().FindUserBySocket( socket );
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

	if( !GameSessionManager::Get().RequestStart( user ) )
	{
		Log::Error( "Failed to start game session [{}]", user->m_gameId );
		return std::make_shared< ResultStartGame >( this, FATAL_ERROR );
	}

	if( !ChatRoomManager::Get().CloseGameChatSession( session->m_gameName ) )
	{
		Log::Error( "Failed to close chat room for game session [{}]", session->m_gameName );
	}

	return std::make_shared< ResultStartGame >( this, SUCCESS );
}

ResultStartGame::ResultStartGame( GenericRequest *request, int32_t reply ) : GenericResponse( *request )
{
	m_reply = reply;
}

void ResultStartGame::Serialize( ByteBuffer &out ) const
{
	out.write_u16( m_packetId );
	out.write_u32( m_trackId );
	out.write_u32( m_reply );
}
