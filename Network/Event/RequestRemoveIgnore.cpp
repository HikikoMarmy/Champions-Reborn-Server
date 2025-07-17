#include "RequestRemoveIgnore.h"

#include "../../Game/RealmUserManager.h"
#include "../../Database/Database.h"
#include "../../logging.h"

void RequestRemoveIgnore::Deserialize( sptr_byte_stream stream )
{
	DeserializeHeader( stream );

	m_sessionId = stream->read_encrypted_utf16();
	m_chatHandle = stream->read_utf16();
}

sptr_generic_response RequestRemoveIgnore::ProcessRequest( sptr_socket socket, sptr_byte_stream stream )
{
	Deserialize( stream );

	auto user = UserManager::Get().FindUserBySocket( socket );
	if( user == nullptr )
	{
		return std::make_shared< ResultRemoveIgnore >( this, FATAL_ERROR );
	}

	if( !user->IsIgnored( m_chatHandle ) )
	{
		return std::make_shared< ResultRemoveIgnore >( this, IGNORE_INVALID );
	}

	if( !Database::Get().RemoveIgnore( user->m_accountId, m_chatHandle ) )
	{
		return std::make_shared< ResultRemoveIgnore >( this, DATABASE_ERROR );
	}

	const auto iter = std::find( user->m_ignoreList.begin(), user->m_ignoreList.end(), m_chatHandle );
	if( iter == user->m_ignoreList.end() )
	{
		return std::make_shared< ResultRemoveIgnore >( this, IGNORE_INVALID );
	}

	user->m_ignoreList.erase( iter );

	return std::make_shared< ResultRemoveIgnore >( this, SUCCESS );
}

ResultRemoveIgnore::ResultRemoveIgnore( GenericRequest *request, int32_t reply ) : GenericResponse( *request )
{
	m_reply = reply;
}

void ResultRemoveIgnore::Serialize( ByteBuffer &out ) const
{
	out.write_u16( m_packetId );
	out.write_u32( m_trackId );
	out.write_u32( m_reply );
}
