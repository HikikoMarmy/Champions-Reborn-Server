#pragma once

#include <memory>
#include <string>

#include "../GenericNetRequest.h"
#include "../GenericNetResponse.h"

class RequestLeaveRoom : public GenericRequest
{
private:
	std::wstring m_roomName;

	enum REPLY {
		SUCCESS = 0,
		GENERAL_ERROR
	};

public:
	static std::unique_ptr< RequestLeaveRoom > Create()
	{
		return std::make_unique< RequestLeaveRoom >();
	}

	sptr_generic_response ProcessRequest( sptr_socket socket, sptr_byte_stream stream ) override;
	void Deserialize( sptr_byte_stream stream ) override;
};

class ResultLeaveRoom : public GenericResponse {
private:
	int32_t m_reply;

public:
	ResultLeaveRoom( GenericRequest *request, int32_t reply );
	void Serialize( ByteBuffer &out ) const;
};