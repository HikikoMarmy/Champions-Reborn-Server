#pragma once

#include <memory>
#include <string>

#include "../GenericNetRequest.h"
#include "../GenericNetResponse.h"

class RequestEnterRoom : public GenericRequest
{
public:
	static std::unique_ptr< RequestEnterRoom > Create()
	{
		return std::make_unique< RequestEnterRoom >();
	}

	sptr_generic_response ProcessRequest( sptr_socket socket, sptr_byte_stream stream ) override;
	void Deserialize( sptr_byte_stream stream ) override;
};

class ResultEnterRoom : public GenericResponse {
public:
	ResultEnterRoom( GenericRequest *request );
	ByteBuffer &Serialize();
};