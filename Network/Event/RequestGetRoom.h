#pragma once

#include <memory>
#include <string>

#include "../GenericNetRequest.h"
#include "../GenericNetResponse.h"

class RequestGetRoom : public GenericRequest {
public:
	static std::unique_ptr< RequestGetRoom > Create()
	{
		return std::make_unique< RequestGetRoom >();
	}

	sptr_generic_response ProcessRequest( sptr_socket socket, sptr_byte_stream stream ) override;
	void Deserialize( sptr_byte_stream stream ) override;
};

class ResultGetRoom : public GenericResponse {
public:
	ResultGetRoom( GenericRequest *request );
	ByteBuffer &Serialize();
};