#pragma once

#include <memory>
#include <string>

#include "../GenericNetRequest.h"
#include "../GenericNetResponse.h"

#include "../../Game/ChatRoomSession.h"

class RequestEnterRoom : public GenericRequest
{
private:
	std::wstring m_roomName;

	enum REPLY {
		SUCCESS = 0,
		GENERAL_ERROR
	};

public:
	static std::unique_ptr< RequestEnterRoom > Create()
	{
		return std::make_unique< RequestEnterRoom >();
	}

	sptr_generic_response ProcessRequest( sptr_socket socket, sptr_byte_stream stream ) override;
	void Deserialize( sptr_byte_stream stream ) override;
};

class ResultEnterRoom : public GenericResponse {
private:
	int32_t m_reply;
	sptr_chat_room_session m_room;

public:
	ResultEnterRoom( GenericRequest *request, int32_t reply, sptr_chat_room_session room );
	void Serialize( ByteBuffer &out ) const;
};