#pragma once

#include <memory>
#include <string>
#include <vector>

#include "RealmUser.h"

class ChatRoomSession {
public:
	ChatRoomSession();
	~ChatRoomSession();

	bool AddMember( sptr_user user );
	bool RemoveMember( sptr_user user );
	bool IsMember( sptr_user user );
	bool IsPublic() const;
	bool IsPrivate() const;

	enum class RoomType {
		Public,
		Private
	};

	RoomType m_type;
	int32_t m_index;

	std::wstring m_name;
	std::wstring m_banner;

	wptr_user m_owner;
	std::vector< wptr_user > m_members;
	std::vector< wptr_user > m_moderators;
};

typedef std::shared_ptr< ChatRoomSession > sptr_chat_room_session;