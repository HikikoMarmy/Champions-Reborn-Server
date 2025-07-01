#include "RequestCancelGame.h"

#include "../../Game/RealmUserManager.h"
#include "../../Game/GameSessionManager.h"
#include "../../logging.h"

void RequestCancelGame::Deserialize( sptr_byte_stream stream )
{
	DeserializeHeader( stream );

	m_sessionId = stream->read_encrypted_utf16();
}

sptr_generic_response RequestCancelGame::ProcessRequest( sptr_socket socket, sptr_byte_stream stream )
{
	Deserialize( stream );

	auto user = UserManager::Get().FindUserBySocket( socket );
	if( user == nullptr )
	{
		Log::Error( "User not found! [{}]", m_sessionId );
		return std::make_shared< ResultCancelGame >( this );
	}

	if( user->m_gameType != RealmGameType::CHAMPIONS_OF_NORRATH )
	{
		return std::make_shared< ResultCancelGame >( this );
	}

	if( !GameSessionManager::Get().RequestCancel( user ) )
	{
		Log::Error( "Failed to cancel game session for user [{}]", user->m_sessionId );
	}

	return std::make_shared< ResultCancelGame >( this );
}

ResultCancelGame::ResultCancelGame( GenericRequest *request ) : GenericResponse( *request )
{

}

void ResultCancelGame::Serialize( ByteBuffer &out ) const
{
	out.write_u16( m_packetId );
	out.write_u32( m_trackId );
	out.write_u32( 0 );
}
