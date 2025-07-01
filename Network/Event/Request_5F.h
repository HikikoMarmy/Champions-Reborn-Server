#pragma once

#include <memory>
#include <string>

#include "../GenericNetRequest.h"
#include "../GenericNetResponse.h"

class Request_5F : public GenericRequest
{
private:
	std::wstring m_sessionId;
	std::wstring m_memberSessionId;

	enum CREATE_REPLY {
		SUCCESS = 0,
		FATAL_ERROR,
		GENERAL_ERROR,
	};
public:
	static std::unique_ptr< Request_5F > Create()
	{
		return std::make_unique< Request_5F >();
	}

	sptr_generic_response ProcessRequest( sptr_socket socket, sptr_byte_stream stream ) override;
	void Deserialize( sptr_byte_stream stream ) override;
};

class Result_5F : public GenericResponse {
private:
	int32_t m_reply;

public:
	Result_5F( GenericRequest *request, int32_t reply );
	ByteBuffer &Serialize();
};