#pragma once

#include <memory>
#include <string>

#include "../GenericNetRequest.h"
#include "../GenericNetResponse.h"

class RequestSendRoomMessage : public GenericRequest
{
private:
	std::wstring m_roomName;
	std::wstring m_message;

	enum REPLY {
		SUCCESS = 0,
		GENERAL_ERROR
	};

public:
	static std::unique_ptr< RequestSendRoomMessage > Create()
	{
		return std::make_unique< RequestSendRoomMessage >();
	}

	sptr_generic_response ProcessRequest( sptr_socket socket, sptr_byte_stream stream ) override;
	void Deserialize( sptr_byte_stream stream ) override;
};

class ResultSendRoomMessage : public GenericResponse {
private:
	int32_t m_reply;

public:
	ResultSendRoomMessage( GenericRequest *request, int32_t reply );
	void Serialize( ByteBuffer &out ) const;
};