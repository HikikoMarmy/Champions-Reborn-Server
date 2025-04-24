#include "../global_define.h"

#include "ChatRoomManager.h"

ChatRoomManager::ChatRoomManager()
{
	m_chatIndex = 0;
	m_chatSessionList.clear();
}

ChatRoomManager::~ChatRoomManager()
{
}

void ChatRoomManager::OnDisconnectUser( sptr_user user )
{
	if( !user || user->m_gameId < 0 )
		return;

	const auto gameId = user->m_gameId;

	auto session = FindRoom( gameId );
	if( !session )
		return;

	//auto owner = session->m_owner.lock();
	//if( !owner )
	//{
	//	Log::Error( "Game session owner not found! [%d]", gameId );
	//	ForceTerminateChat( gameId );
	//	return;
	//}
	//
	//if( owner->m_sessionId == user->m_sessionId )
	//{
	//	Log::Info( "Game session owner disconnected! [%d]", gameId );
	//	ForceTerminateChat( gameId );
	//}
}

bool ChatRoomManager::CreatePublicChatSession( sptr_user owner, std::wstring gameName )
{
	auto new_session = std::make_shared< ChatRoomSession >();

	new_session->m_type = ChatRoomSession::GameType::Public;
	new_session->m_gameIndex = m_chatIndex;
	new_session->m_gameName = L"";
	
	std::lock_guard< std::mutex > lock( m_mutex );
	m_chatSessionList.push_back( new_session );

	m_chatIndex++;

	return true;
}

bool ChatRoomManager::CreatePrivateChatSession( sptr_user owner, std::wstring roomName )
{
	for( auto &chatSession : m_chatSessionList )
	{
		if( chatSession->m_type != ChatRoomSession::GameType::Private )
			continue;

		if( chatSession->m_gameName == roomName )
		{
			Log::Error( "Game name is already in use! [%S]", roomName.c_str() );
			return false;
		}
	}

	auto new_session = std::make_shared< ChatRoomSession >();

	new_session->m_type = ChatRoomSession::GameType::Private;
	new_session->m_gameIndex = m_chatIndex;
	new_session->m_gameName = roomName;
	
	std::lock_guard< std::mutex > lock( m_mutex );
	m_chatSessionList.push_back( new_session );

	m_chatIndex++;

	return true;
}

bool ChatRoomManager::ForceTerminateChat( const int32_t gameId )
{
	if( gameId < 0 )
	{
		return false;
	}

	std::lock_guard< std::mutex > lock( m_mutex );

	auto it = std::find_if( m_chatSessionList.begin(), m_chatSessionList.end(), [ &gameId ]( sptr_chat_room_session chatSession )
	{
		return chatSession->m_gameIndex == gameId;
	} );

	if( it != m_chatSessionList.end() )
	{
		m_chatSessionList.erase( it );
		return true;
	}

	return false;
}

sptr_chat_room_session ChatRoomManager::FindRoom( const int32_t gameId )
{
	if( gameId < 0 ) return nullptr;

	std::lock_guard< std::mutex > lock( m_mutex );

	for( auto &chatSession : m_chatSessionList )
	{
		if( chatSession->m_gameIndex == gameId )
		{
			return chatSession;
		}
	}

	return nullptr;
}

sptr_chat_room_session ChatRoomManager::FindRoom( const std::wstring &gameName )
{
	if( gameName.empty() ) return nullptr;

	std::lock_guard< std::mutex > lock( m_mutex );

	for( auto &chatSession : m_chatSessionList )
	{
		if( chatSession->m_gameName == gameName )
		{
			return chatSession;
		}
	}

	return nullptr;
}