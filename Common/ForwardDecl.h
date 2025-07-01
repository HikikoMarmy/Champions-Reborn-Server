#pragma once

#include <memory>

class RealmUser;
using sptr_user = std::shared_ptr< RealmUser >;
using wptr_user = std::weak_ptr< RealmUser >;

class GameSession;
using sptr_game_session = std::shared_ptr< GameSession >;

class RealmSocket;
using sptr_socket = std::shared_ptr< RealmSocket >;

class ByteBuffer;
using sptr_byte_stream = std::shared_ptr< ByteBuffer >;

class ChatRoomSession;
using sptr_chat_room_session = std::shared_ptr< ChatRoomSession >;

class RealmCharacter;
using sptr_realm_character = std::shared_ptr< RealmCharacter >;