#include "RequestCancelGame_RTA.h"

#include "../../Game/RealmUserManager.h"
#include "../../Game/GameSessionManager.h"
#include "../../logging.h"

void RequestCancelGame_RTA::Deserialize( sptr_byte_stream stream )
{
	DeserializeHeader( stream );

	m_sessionId = stream->read_encrypted_utf16();
}

sptr_generic_response RequestCancelGame_RTA::ProcessRequest( sptr_socket socket, sptr_byte_stream stream )
{
	Deserialize( stream );

	auto user = RealmUserManager::Get().FindUserBySocket( socket );

	if( user == nullptr )
	{
		Log::Error( "User not found! [%S]", m_sessionId.c_str() );
		return std::make_shared< ResultCancelGame_RTA >( this );
	}

	GameSessionManager::Get().RequestCancel( user );

	return std::make_shared< ResultCancelGame_RTA >( this );
}

ResultCancelGame_RTA::ResultCancelGame_RTA( GenericRequest *request ) : GenericResponse( *request )
{
	
}

ByteBuffer &ResultCancelGame_RTA::Serialize()
{
	m_stream.write_u16( m_packetId );
	m_stream.write_u32( m_trackId );
	m_stream.write_u32( 0 );

	return m_stream;
}
