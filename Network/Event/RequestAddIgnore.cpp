#include "RequestAddIgnore.h"

#include "../../Game/RealmUserManager.h"
#include "../../Database/Database.h"
#include "../../logging.h"

void RequestAddIgnore::Deserialize( sptr_byte_stream stream )
{
	DeserializeHeader( stream );

	m_sessionId = stream->read_encrypted_utf16();
	m_chatHandle = stream->read_utf16();
}

sptr_generic_response RequestAddIgnore::ProcessRequest( sptr_socket socket, sptr_byte_stream stream )
{
	Deserialize( stream );

	auto user = UserManager::Get().FindUserBySocket( socket );
	if( user == nullptr )
	{
		return std::make_shared< ResultAddIgnore >( this, FATAL_ERROR );
	}

	if( user->IsFriend( m_chatHandle ) )
	{
		return std::make_shared< ResultAddIgnore >( this, IGNORE_FRIEND );
	}

	if( user->IsIgnored( m_chatHandle ) )
	{
		return std::make_shared< ResultAddIgnore >( this, IGNORE_DUPLICATE );
	}

	auto targetUser = UserManager::Get().FindUserByChatHandle( m_chatHandle );
	if( targetUser == nullptr )
	{
		return std::make_shared< ResultAddIgnore >( this, IGNORE_INVALID );
	}

	if( !Database::Get().SaveIgnore( user->m_accountId, targetUser->m_chatHandle ) )
	{
		return std::make_shared< ResultAddIgnore >( this, DATABASE_ERROR );
	}

	user->m_ignoreList.push_back( targetUser->m_chatHandle );

	return std::make_shared< ResultAddIgnore >( this, SUCCESS );
}

ResultAddIgnore::ResultAddIgnore( GenericRequest *request, int32_t reply ) : GenericResponse( *request )
{
	m_reply = reply;
}

void ResultAddIgnore::Serialize( ByteBuffer &out ) const
{
	out.write_u16( m_packetId );
	out.write_u32( m_trackId );
	out.write_u32( m_reply );
}
