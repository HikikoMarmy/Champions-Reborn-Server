#pragma once

#include <memory>
#include <string>

#include "../GenericNetRequest.h"
#include "../GenericNetResponse.h"

#include "../../Game/ChatRoomSession.h"

class RequestGetRoom : public GenericRequest {
private:
	std::wstring m_sessionId;
	std::wstring m_roomName;

	enum CREATE_ACCOUNT_REPLY {
		SUCCESS = 0,
		GENERAL_ERROR = 1,
	};

public:
	static std::unique_ptr< RequestGetRoom > Create()
	{
		return std::make_unique< RequestGetRoom >();
	}

	sptr_generic_response ProcessRequest( sptr_socket socket, sptr_byte_stream stream ) override;
	void Deserialize( sptr_byte_stream stream ) override;
};

class ResultGetRoom : public GenericResponse {
private:
	int32_t m_reply;
	sptr_chat_room_session m_room;

public:
	ResultGetRoom( GenericRequest *request, int32_t reply, sptr_chat_room_session room = nullptr );
	void Serialize( ByteBuffer &out ) const;
};