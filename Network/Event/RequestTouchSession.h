#pragma once

#include <memory>
#include <string>

#include "../GenericNetRequest.h"
#include "../GenericNetResponse.h"

class RequestTouchSession : public GenericRequest
{
private:
	std::wstring m_sessionId;

public:
	static std::unique_ptr< RequestTouchSession > Create()
	{
		return std::make_unique< RequestTouchSession >();
	}
	sptr_generic_response ProcessRequest( sptr_socket socket, sptr_byte_stream stream ) override;
	void Deserialize( sptr_byte_stream stream ) override;
};

class ResultTouchSession : public GenericResponse {
public:
	ResultTouchSession( GenericRequest *request );
	ByteBuffer &Serialize();
};