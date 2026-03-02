#pragma once

#include <memory>
#include <string>

#include "../GenericNetRequest.h"
#include "../GenericNetResponse.h"

class Request_61 : public GenericRequest
{
public:
	static std::unique_ptr< Request_61 > Create()
	{
		return std::make_unique< Request_61 >();
	}

	sptr_generic_response ProcessRequest( sptr_socket socket, sptr_byte_stream stream ) override;
	void Deserialize( sptr_byte_stream stream ) override;
};

class Result_61 : public GenericResponse {
public:
	Result_61( GenericRequest *request );
	ByteBuffer &Serialize();
};