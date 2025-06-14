#pragma once

#include <memory>
#include <mutex>
#include <vector>
#include <string>

#include "ChatRoomSession.h"

class ChatRoomManager {
private:
	static inline std::unique_ptr< ChatRoomManager > m_instance;
	static inline std::mutex m_mutex;

	int32_t m_chatIndex;
	std::vector< sptr_chat_room_session > m_chatSessionList;

public:
	ChatRoomManager();
	~ChatRoomManager();
	ChatRoomManager( const ChatRoomManager & ) = delete;
	ChatRoomManager &operator=( const ChatRoomManager & ) = delete;

	static ChatRoomManager &Get()
	{
		std::lock_guard< std::mutex > lock( m_mutex );
		if( m_instance == nullptr )
		{
			m_instance.reset( new ChatRoomManager() );
		}

		return *m_instance;
	}

	void OnDisconnectUser( sptr_user user );

	bool CreatePublicChatSession( sptr_user user, std::wstring roomName );
	bool CreatePrivateChatSession( sptr_user user, std::wstring roomName );
	bool ForceTerminateChat( const int32_t gameId );
	sptr_chat_room_session FindRoom( const int32_t gameId );
	sptr_chat_room_session FindRoom( const std::wstring &roomName );
};