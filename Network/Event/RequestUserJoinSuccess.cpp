#include "RequestUserJoinSuccess.h"

#include "../../Game/GameSessionManager.h"
#include "../../Game/RealmUserManager.h"
#include "../../Game/RealmUser.h"
#include "../../Database/Database.h"
#include "../../logging.h"

void RequestUserJoinSuccess::Deserialize( sptr_byte_stream stream )
{
	DeserializeHeader( stream );

	m_sessionId = stream->read_encrypted_utf16();
	m_ownerSessionId = stream->read_encrypted_utf16();
}

sptr_generic_response RequestUserJoinSuccess::ProcessRequest( sptr_socket socket, sptr_byte_stream stream )
{
	Deserialize( stream );

	auto user = UserManager::Get().FindUserBySocket( socket );
	if( user == nullptr )
	{
		return std::make_shared< ResultUserJoinSuccess >( this, FATAL_ERROR );
	}
	
	auto gameSession = GameSessionManager::Get().FindGame( user->m_gameId, user->m_gameType );
	if( gameSession == nullptr )
	{
		Log::Error( "Game session not found for user: {}", user->m_username );
		return std::make_shared< ResultUserJoinSuccess >( this, FATAL_ERROR );
	}
	
	if( !gameSession->IsJoinable() )
	{
		Log::Error( "Game session is not open for user: {}", user->m_username );
		return std::make_shared< ResultUserJoinSuccess >( this, FATAL_ERROR );
	}
	
	if( !gameSession->AddMember( user ) )
	{
		Log::Error( "Failed to add user {} to game session {}", user->m_username, gameSession->m_gameId );
		return std::make_shared< ResultUserJoinSuccess >( this, FATAL_ERROR );
	}

	return std::make_shared< ResultUserJoinSuccess >( this, SUCCESS );
}

ResultUserJoinSuccess::ResultUserJoinSuccess( GenericRequest *request, int32_t reply ) : GenericResponse( *request )
{
	m_reply = reply;
}

void ResultUserJoinSuccess::Serialize( ByteBuffer &out ) const
{
	out.write_u16( m_packetId );
	out.write_u32( m_trackId );
	out.write_u32( m_reply );
}
