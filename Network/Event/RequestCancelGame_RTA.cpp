#include "RequestCancelGame_RTA.h"

#include "../../Game/RealmUserManager.h"
#include "../../Game/GameSessionManager.h"
#include "../../Game/ChatRoomManager.h"
#include "../../logging.h"

void RequestCancelGame_RTA::Deserialize( sptr_byte_stream stream )
{
	DeserializeHeader( stream );

	m_sessionId = stream->read_encrypted_utf16();
}

sptr_generic_response RequestCancelGame_RTA::ProcessRequest( sptr_socket socket, sptr_byte_stream stream )
{
	Deserialize( stream );

	auto user = UserManager::Get().FindUserBySocket( socket );
	if( user == nullptr )
	{
		Log::Error( "User not found! [{}]", m_sessionId );
		return std::make_shared< ResultCancelGame_RTA >( this );
	}

	if( user->m_gameType != RealmGameType::RETURN_TO_ARMS )
	{
		return std::make_shared< ResultCancelGame_RTA >( this );
	}

	const auto &gameSession = GameSessionManager::Get().FindGame( user->m_gameId, user->m_gameType );
	if( gameSession == nullptr )
	{
		Log::Error( "Game session not found for user [{}]", user->m_sessionId );
		return std::make_shared< ResultCancelGame_RTA >( this );
	}

	if( !GameSessionManager::Get().RequestCancel( user ) )
	{
		Log::Error( "Failed to cancel game session for user [{}]", user->m_sessionId );
	}

	if( !ChatRoomManager::Get().LeaveRoom( user, user->m_privateRoomId ) )
	{
		Log::Error( "Failed to leave private chat room for user [{}]", user->m_username );
	}

	return std::make_shared< ResultCancelGame_RTA >( this );
}

ResultCancelGame_RTA::ResultCancelGame_RTA( GenericRequest *request ) : GenericResponse( *request )
{
	
}

void ResultCancelGame_RTA::Serialize( ByteBuffer &out ) const
{
	out.write_u16( m_packetId );
	out.write_u32( m_trackId );
	out.write_u32( 0 );
}
