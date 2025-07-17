#include "RequestAddFriend.h"

#include "../../Game/RealmUserManager.h"
#include "../../Database/Database.h"
#include "../../logging.h"

void RequestAddFriend::Deserialize( sptr_byte_stream stream )
{
	DeserializeHeader( stream );

	m_sessionId = stream->read_encrypted_utf16();
	m_chatHandle = stream->read_utf16();
}

sptr_generic_response RequestAddFriend::ProcessRequest( sptr_socket socket, sptr_byte_stream stream )
{
	Deserialize( stream );

	auto user = UserManager::Get().FindUserBySocket( socket );
	if( user == nullptr )
	{
		return std::make_shared< ResultAddFriend >( this, FATAL_ERROR );
	}

	if( user->IsFriend( m_chatHandle ) )
	{
		return std::make_shared< ResultAddFriend >( this, FRIEND_DUPLICATE );
	}

	auto targetUser = UserManager::Get().FindUserByChatHandle( m_chatHandle );
	if( targetUser == nullptr )
	{
		return std::make_shared< ResultAddFriend >( this, FRIEND_INVALID );
	}

	if( targetUser->IsIgnored( user->m_chatHandle ) )
	{
		return std::make_shared< ResultAddFriend >( this, FRIEND_IGNORING );
	}

	if( !Database::Get().SaveFriend( user->m_accountId, targetUser->m_chatHandle ) )
	{
		return std::make_shared< ResultAddFriend >( this, DATABASE_ERROR );
	}

	user->m_friendList.push_back( targetUser->m_chatHandle );

	return std::make_shared< ResultAddFriend >( this, SUCCESS );
}

ResultAddFriend::ResultAddFriend( GenericRequest *request, int32_t reply ) : GenericResponse( *request )
{
	m_reply = reply;
}

void ResultAddFriend::Serialize( ByteBuffer &out ) const
{
	out.write_u16( m_packetId );
	out.write_u32( m_trackId );
	out.write_u32( m_reply );
}
