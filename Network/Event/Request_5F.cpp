#include "Request_5F.h"

#include "../../Game/RealmUserManager.h"
#include "../../Game/RealmUser.h"
#include "../../Database/Database.h"
#include "../../logging.h"

void Request_5F::Deserialize( sptr_byte_stream stream )
{
	DeserializeHeader( stream );

	auto sessionId_a = stream->read_encrypted_utf16();
	auto sessionId_b = stream->read_encrypted_utf16();
}

sptr_generic_response Request_5F::ProcessRequest( sptr_socket socket, sptr_byte_stream stream )
{
	Deserialize( stream );

	auto user = RealmUserManager::Get().FindUserBySocket( socket );
	if( user == nullptr )
	{
		return std::make_shared< Result_5F >( this, FATAL_ERROR );
	}

	Log::Debug( "Request_5F From user ID : %d", user->m_accountId );

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
