#pragma once

#include <memory>
#include <mutex>
#include <string>
#include <map>

#include "ChatRoomSession.h"

class ChatRoomManager {
private:
	int32_t m_roomIndex = 0;
	std::map< int32_t, sptr_chat_room_session > m_chatSessionList;

	void CreatePublicRooms();
public:
	ChatRoomManager();
	~ChatRoomManager();
	ChatRoomManager( const ChatRoomManager & ) = delete;
	ChatRoomManager &operator=( const ChatRoomManager & ) = delete;

	static ChatRoomManager &Get()
	{
		static ChatRoomManager instance;
		return instance;
	}

	std::vector< sptr_chat_room_session > GetPublicRoomList() const;

	bool JoinRoom( sptr_user user, const std::wstring &roomName );
	bool LeaveRoom( sptr_user user, const std::wstring &roomName );
	bool LeaveRoom( sptr_user user, const int32_t roomId );

	void OnDisconnectUser( sptr_user user );
	bool CreateGameChatSession( sptr_user user, const std::wstring roomName );
	bool CloseGameChatSession( const std::wstring roomName );

	void SendMessageToRoom( const std::wstring roomName, const std::wstring handle, const std::wstring message );

	sptr_chat_room_session FindRoom( const std::wstring &roomName );
	sptr_chat_room_session FindRoom( const int32_t roomId );
};