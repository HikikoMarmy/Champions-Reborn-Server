#include "Request_5F.h"

#include "../../Game/GameSessionManager.h"
#include "../../Game/RealmUserManager.h"
#include "../../Game/RealmUser.h"
#include "../../Database/Database.h"
#include "../../logging.h"

void Request_5F::Deserialize( sptr_byte_stream stream )
{
	DeserializeHeader( stream );

	m_sessionId = stream->read_encrypted_utf16();
	m_memberSessionId = stream->read_encrypted_utf16();
}

sptr_generic_response Request_5F::ProcessRequest( sptr_socket socket, sptr_byte_stream stream )
{
	Deserialize( stream );

	Log::Packet( stream->get_buffer(), stream->get_length(), false );

	//auto user = UserManager::Get().FindUserBySocket( socket );
	//if( user == nullptr )
	//{
	//	return std::make_shared< Result_5F >( this, FATAL_ERROR );
	//}
	//
	//auto targetUser = UserManager::Get().FindUserBySessionId( m_memberSessionId );
	//if( targetUser == nullptr || targetUser->m_accountId == -1 )
	//{
	//	Log::Error( "Target user not found or invalid account ID for session: %S", m_memberSessionId.c_str() );
	//	return std::make_shared< Result_5F >( this, FATAL_ERROR );
	//}
	//
	//auto gameSession = GameSessionManager::Get().FindGame( user->m_gameId, user->m_gameType );
	//if( gameSession == nullptr )
	//{
	//	Log::Error( "Game session not found for user: %S", user->m_sessionId.c_str() );
	//	return std::make_shared< Result_5F >( this, FATAL_ERROR );
	//}
	//
	//if( !gameSession->IsJoinable() )
	//{
	//	Log::Error( "Game session is not open for user: %S", user->m_sessionId.c_str() );
	//	return std::make_shared< Result_5F >( this, FATAL_ERROR );
	//}
	//
	//if( !gameSession->AddMember( targetUser ) )
	//{
	//	Log::Error( "Failed to add user %S to game session %d", targetUser->m_sessionId.c_str(), gameSession->m_gameIndex );
	//	return std::make_shared< Result_5F >( this, FATAL_ERROR );
	//}

	// TODO: Here is more like finalize/confirm joined

	return std::make_shared< Result_5F >( this, SUCCESS );
}

Result_5F::Result_5F( GenericRequest *request, int32_t reply ) : GenericResponse( *request )
{
	m_reply = reply;
}

ByteBuffer& Result_5F::Serialize()
{
	m_stream.write_u16( m_packetId );
	m_stream.write_u32( m_trackId );
	m_stream.write_u32( m_reply );

	return m_stream;
}
