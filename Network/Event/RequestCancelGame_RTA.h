#pragma once

#include <memory>
#include <string>

#include "../GenericNetRequest.h"
#include "../GenericNetResponse.h"

class RequestCancelGame_RTA : public GenericRequest
{
private:
	std::wstring m_sessionId;

public:
	static std::unique_ptr< RequestCancelGame_RTA > Create()
	{
		return std::make_unique< RequestCancelGame_RTA >();
	}

	sptr_generic_response ProcessRequest( sptr_socket socket, sptr_byte_stream stream ) override;
	void Deserialize( sptr_byte_stream stream ) override;
};

class ResultCancelGame_RTA : public GenericResponse {
public:
	ResultCancelGame_RTA( GenericRequest *request );
	ByteBuffer &Serialize();
};