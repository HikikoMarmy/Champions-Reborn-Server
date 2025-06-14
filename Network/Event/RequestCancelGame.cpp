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

	auto user = RealmUserManager::Get().FindUserBySocket( socket );

	if( user == nullptr )
	{
		Log::Error( "User not found! [%S]", m_sessionId.c_str() );
		return std::make_shared< ResultCancelGame >( this );
	}

	if (user->m_isHost)
	{
		auto result = GameSessionManager::Get().RequestCancel(user);

		if( false == result )
		{
			Log::Error( "Failed to cancel game session for user [%S]", user->m_sessionId.c_str() );
		}
	}
	else
	{
		user->m_isHost = false;
		user->m_gameId = -1;
	}

	return std::make_shared< ResultCancelGame >( this );
}

ResultCancelGame::ResultCancelGame( GenericRequest *request ) : GenericResponse( *request )
{
	
}

ByteBuffer &ResultCancelGame::Serialize()
{
	m_stream.write_u16( m_packetId );
	m_stream.write_u32( m_trackId );
	m_stream.write_u32( 0 );

	return m_stream;
}
