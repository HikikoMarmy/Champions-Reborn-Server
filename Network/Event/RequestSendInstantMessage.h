#pragma once

#include <memory>
#include <string>

#include "../GenericNetRequest.h"
#include "../GenericNetResponse.h"

class RequestSendInstantMessage : public GenericRequest
{
private:
	std::wstring m_chatHandle;
	std::wstring m_message;

	enum ERROR_CODE {
		SUCCESS = 0,
		GENERAL_ERROR,
		USER_IGNORED = 19,
	};

public:
	static std::unique_ptr< RequestSendInstantMessage > Create()
	{
		return std::make_unique< RequestSendInstantMessage >();
	}

	sptr_generic_response ProcessRequest( sptr_socket socket, sptr_byte_stream stream ) override;
	void Deserialize( sptr_byte_stream stream ) override;
};

class ResultSendInstantMessage : public GenericResponse {
private:
	int32_t m_reply;

public:
	ResultSendInstantMessage( GenericRequest *request, int32_t reply );
	void Serialize( ByteBuffer &out ) const;
};