#include "ChatRoomSession.h"

#include "RealmUser.h"

ChatRoomSession::ChatRoomSession()
{
	m_type = RoomType::Public;
	m_index = 0;
	m_name.clear();
}

ChatRoomSession::~ChatRoomSession()
{
	m_type = RoomType::Public;
	m_index = 0;
	m_name.clear();
}

bool ChatRoomSession::AddMember( sptr_user user )
{
	if( !user )
		return false;

	for( const auto &member : m_members )
	{
		if( member.lock() == user )
			return false; // User already in the room.
	}

	m_members.push_back( user );

	return true;
}

bool ChatRoomSession::RemoveMember( sptr_user user )
{
	if( !user )
		return false;

	auto it = std::remove_if( m_members.begin(), m_members.end(),
							  [ &user ]( const std::weak_ptr< RealmUser > &member )
	{
		return member.lock() == user;
	} );

	if( it == m_members.end() )
	{
		return false;
	}

	m_members.erase( it, m_members.end() );

	return true;
}

bool ChatRoomSession::IsMember( sptr_user user )
{
	if( !user )
		return false;

	for( const auto &member : m_members )
	{
		if( member.lock() == user )
			return true;
	}
	return false;
}

bool ChatRoomSession::IsPublic() const
{
	return m_type == RoomType::Public;
}

bool ChatRoomSession::IsPrivate() const
{
	return m_type == RoomType::Private;
}
