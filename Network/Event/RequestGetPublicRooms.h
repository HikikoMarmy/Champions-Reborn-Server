#pragma once

#include <memory>
#include <string>

#include "../GenericNetRequest.h"
#include "../GenericNetResponse.h"

#include "../../Game/ChatRoomSession.h"

class RequestGetPublicRooms : public GenericRequest
{
public:
	static std::unique_ptr< RequestGetPublicRooms > Create()
	{
		return std::make_unique< RequestGetPublicRooms >();
	}

	sptr_generic_response ProcessRequest( sptr_socket socket, sptr_byte_stream stream ) override;
	void Deserialize( sptr_byte_stream stream ) override;
};

class ResultGetPublicRooms : public GenericResponse {
private:
	std::vector< sptr_chat_room_session > m_rooms;

public:
	ResultGetPublicRooms( GenericRequest *request, std::vector< sptr_chat_room_session > rooms );
	void Serialize( ByteBuffer &out ) const;
};