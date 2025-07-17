#include "RequestRemoveFriend.h"

#include "../../Game/RealmUserManager.h"
#include "../../Database/Database.h"
#include "../../logging.h"

void RequestRemoveFriend::Deserialize( sptr_byte_stream stream )
{
	DeserializeHeader( stream );

	m_sessionId = stream->read_encrypted_utf16();
	m_chatHandle = stream->read_utf16();
}

sptr_generic_response RequestRemoveFriend::ProcessRequest( sptr_socket socket, sptr_byte_stream stream )
{
	Deserialize( stream );

	auto user = UserManager::Get().FindUserBySocket( socket );
	if( user == nullptr )
	{
		return std::make_shared< ResultRemoveFriend >( this, FATAL_ERROR );
	}

	if( !user->IsFriend( m_chatHandle ) )
	{
		return std::make_shared< ResultRemoveFriend >( this, FRIEND_INVALID );
	}

	if( !Database::Get().RemoveFriend( user->m_accountId, m_chatHandle ) )
	{
		return std::make_shared< ResultRemoveFriend >( this, DATABASE_ERROR );
	}

	const auto iter = std::find( user->m_friendList.begin(), user->m_friendList.end(), m_chatHandle );
	if( iter == user->m_friendList.end() )
	{
		return std::make_shared< ResultRemoveFriend >( this, FRIEND_INVALID );
	}

	user->m_friendList.erase( iter );

	return std::make_shared< ResultRemoveFriend >( this, SUCCESS );
}

ResultRemoveFriend::ResultRemoveFriend( GenericRequest *request, int32_t reply ) : GenericResponse( *request )
{
	m_reply = reply;
}

void ResultRemoveFriend::Serialize( ByteBuffer &out ) const
{
	out.write_u16( m_packetId );
	out.write_u32( m_trackId );
	out.write_u32( m_reply );
}
