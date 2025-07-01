#include "ChatRoomManager.h"

#include "RealmUser.h"

#include "../Network/Event/NotifyRoomMessage.h"
#include "../logging.h"

ChatRoomManager::ChatRoomManager()
{
	m_chatSessionList.clear();
	m_roomIndex = 0;

	CreatePublicRooms();
}

ChatRoomManager::~ChatRoomManager()
{
}

void ChatRoomManager::CreatePublicRooms()
{
	std::vector< std::wstring > RoomNames = {
		L"Champions Reborn",
		L"Adventurous",
		L"Courageous",
		L"Champion",
		L"Legendary",
		L"Epic"
	};

	for( const auto &name : RoomNames )
	{
		auto new_session = std::make_shared< ChatRoomSession >();

		new_session->m_name = name;
		new_session->m_type = ChatRoomSession::RoomType::Public;
		new_session->m_index = m_roomIndex;
		new_session->m_banner = L"";

		m_chatSessionList[ m_roomIndex++ ] = new_session;
	}
}

std::vector<sptr_chat_room_session> ChatRoomManager::GetPublicRoomList() const
{
	std::vector< sptr_chat_room_session > result;

	for( const auto &chatSession : m_chatSessionList )
	{
		if( chatSession.second->m_type == ChatRoomSession::RoomType::Public )
		{
			result.push_back( chatSession.second );
		}
	}

	return result;
}

bool ChatRoomManager::JoinRoom( sptr_user user, const std::wstring &roomName )
{
	if( roomName.empty() || !user )
		return false;

	auto chatSession = FindRoom( roomName );
	if( !chatSession )
	{
		Log::Error( "Chat room [{}] not found", roomName );
		return false;
	}

	if( !chatSession->AddMember( user ) )
	{
		Log::Error( "Failed to add user [{}] to chat room [{}]", user->m_username, roomName );
		return false;
	}

	if( chatSession->m_type == ChatRoomSession::RoomType::Public )
	{
		user->m_publicRoomId = chatSession->m_index;
	}
	else
	{
		user->m_privateRoomId = chatSession->m_index;

		SendMessageToRoom( roomName, L"", L"User '" + user->m_chatHandle + L"' has joined the room." );
	}

	Log::Info( "User [{}] joined chat room [{}]", user->m_username, roomName );

	return true;
}

bool ChatRoomManager::LeaveRoom( sptr_user user, const std::wstring &roomName )
{
	if( !user || roomName.empty() )
		return false;

	auto chatSession = FindRoom( roomName );
	if( !chatSession )
	{
		Log::Error( "Chat room [{}] not found", roomName );
		return false;
	}

	if( !chatSession->RemoveMember( user ) )
	{
		Log::Error( "Failed to remove user [{}] from chat room [{}]", user->m_username, roomName );
		return false;
	}

	if( chatSession->m_type == ChatRoomSession::RoomType::Private )
	{
		if( chatSession->m_members.empty() && chatSession->m_moderators.empty() )
		{
			m_chatSessionList.erase( chatSession->m_index );

			Log::Debug( "Private chat room [{}] deleted", roomName );
		}

		user->m_privateRoomId = -1;
	}
	else
	{
		user->m_publicRoomId = -1;
	}

	Log::Info( "User [{}] left chat room [{}]", user->m_username, roomName );
	return true;
}

bool ChatRoomManager::LeaveRoom( sptr_user user, const int32_t roomId )
{
	if( !user || roomId < 0 )
		return false;

	const auto chatSession = FindRoom( roomId );
	if( !chatSession )
	{
		Log::Error( "Chat room with ID [{}] not found", roomId );
		return false;
	}

	if( !chatSession->RemoveMember( user ) )
	{
		Log::Error( "Failed to remove user [{}] from chat room with ID [{}]", user->m_username, roomId );
		return false;
	}

	if( chatSession->m_type == ChatRoomSession::RoomType::Private )
	{
		if( chatSession->m_members.empty() && chatSession->m_moderators.empty() )
		{
			m_chatSessionList.erase( roomId );
			Log::Info( "Private chat room with ID [{}] deleted", roomId );
		}

		user->m_privateRoomId = -1;
	}
	else
	{
		user->m_publicRoomId = -1;
	}

	Log::Info( "User [{}] left chat room with ID [{}]", user->m_username, roomId );

	return true;
}

void ChatRoomManager::OnDisconnectUser( sptr_user user )
{
	if( !user )	return;

	LeaveRoom( user, user->m_publicRoomId );
	LeaveRoom( user, user->m_privateRoomId );
}

bool ChatRoomManager::CreateGameChatSession( sptr_user owner, std::wstring roomName )
{
	for( const auto &chatSession : m_chatSessionList )
	{
		if( chatSession.second->m_name == roomName )
		{
			Log::Error( "Chat Room name is already in use! [{}]", roomName );
			return false;
		}
	}

	auto new_session = std::make_shared< ChatRoomSession >();

	new_session->m_type = ChatRoomSession::RoomType::Private;
	new_session->m_name = roomName;
	new_session->m_owner = owner;
	new_session->m_banner = L"";
	new_session->m_index = m_roomIndex;

	new_session->AddMember( owner );
	owner->m_privateRoomId = m_roomIndex;

	m_chatSessionList[ m_roomIndex++ ] = new_session;

	return true;
}

bool ChatRoomManager::CloseGameChatSession( const std::wstring roomName )
{
	if( roomName.empty() )
		return false;

	const auto it = std::find_if( m_chatSessionList.begin(), m_chatSessionList.end(),
								  [ &roomName ]( const auto &pair )
	{
		return pair.second->m_name == roomName;
	} );

	if( it == m_chatSessionList.end() )
	{
		Log::Error( "Chat room [{}] not found", roomName );
		return false;
	}

	auto &chatSession = it->second;
	if( chatSession->m_type != ChatRoomSession::RoomType::Private )
	{
		Log::Error( "Chat room [{}] is not a private room", roomName );
		return false;
	}

	for( const auto &member : chatSession->m_members )
	{
		if( auto user = member.lock() )
		{
			user->m_privateRoomId = -1;
		}
	}

	m_chatSessionList.erase( it );

	Log::Info( "Chat room [{}] closed", roomName );

	return true;
}

void ChatRoomManager::SendMessageToRoom( std::wstring roomName, std::wstring handle, std::wstring message )
{
	if( roomName.empty() || message.empty() )
		return;

	auto chatSession = FindRoom( roomName );
	if( !chatSession )
	{
		Log::Error( "Chat room [{}] not found", roomName );
		return;
	}

	NotifyRoomMessage notifyMessage( roomName, handle, message );
	for( const auto &m : chatSession->m_members )
	{
		if( auto member = m.lock() )
		{
			member->sock->send( notifyMessage );
		}
	}
}

sptr_chat_room_session ChatRoomManager::FindRoom( const std::wstring &gameName )
{
	if( gameName.empty() )
		return nullptr;

	for( const auto &chatSession : m_chatSessionList )
	{
		if( chatSession.second->m_name == gameName )
		{
			return chatSession.second;
		}
	}

	return nullptr;
}

sptr_chat_room_session ChatRoomManager::FindRoom( const int32_t roomId )
{
	if( roomId < 0 )
		return nullptr;

	auto it = m_chatSessionList.find( roomId );

	if( it != m_chatSessionList.end() )
	{
		return it->second;
	}

	Log::Error( "Chat room with ID [{}] not found", roomId );
	return nullptr;
}
