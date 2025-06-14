#pragma once

#include <memory>
#include <string>
#include "RealmUser.h"

class ChatRoomSession {
public:
	ChatRoomSession();
	~ChatRoomSession();

	enum class GameType
	{
		Public,
		Private
	} m_type;

	int32_t m_gameIndex;
	std::wstring m_gameName;

	std::weak_ptr< RealmUser > m_owner;
};

typedef std::shared_ptr< ChatRoomSession > sptr_chat_room_session;