#include "RequestGetSocialListInitial.h"

#include "../../Game/RealmUserManager.h"
#include "../../Game/RealmUser.h"
#include "../../logging.h"

void RequestGetSocialListInitial::Deserialize( sptr_byte_stream stream )
{
	DeserializeHeader( stream );
}

sptr_generic_response RequestGetSocialListInitial::ProcessRequest( sptr_socket socket, sptr_byte_stream stream )
{
	Deserialize( stream );

	auto user = UserManager::Get().FindUserBySocket( socket );
	if( user == nullptr )
	{
		Log::Error( "RequestGetFriendList::ProcessRequest() - User not found" );
		return std::make_shared< ResultGetSocialListInitial >( this );
	}

	return std::make_shared< ResultGetSocialListInitial >( this, user );
}

ResultGetSocialListInitial::ResultGetSocialListInitial( GenericRequest *request, sptr_user user ) : GenericResponse( *request )
{
	m_user = user;
}

void ResultGetSocialListInitial::Serialize( ByteBuffer &out ) const
{
	out.write_u16( m_packetId );
	out.write_u32( m_trackId );
	out.write_u32( 0 );

	// Friends
	out.write_u32( static_cast< uint32_t >( m_user->m_friendList.size() ) );
	for( const auto &friendHandle : m_user->m_friendList )
	{
		out.write_utf16( friendHandle );
	}

	// ?
	out.write_u32(1);
	out.write_utf16(L"String_2");

	// Ignore
	out.write_u32( static_cast< uint32_t >( m_user->m_ignoreList.size() ) );
	for( const auto &ignoreHandle : m_user->m_ignoreList )
	{
		out.write_utf16( ignoreHandle );
	}
}
