#pragma once

#include <memory>
#include <string>

#include "../GenericNetRequest.h"
#include "../GenericNetResponse.h"

class RequestGetPublicRooms : public GenericRequest
{
public:
	static std::unique_ptr< RequestGetPublicRooms > Create()
	{
		return std::make_unique< RequestGetPublicRooms >();
	}

	sptr_generic_response ProcessRequest( sptr_socket socket, sptr_byte_stream stream ) override;
	void Deserialize( sptr_byte_stream stream ) override;
};

class ResultGetPublicRooms : public GenericResponse {
public:
	ResultGetPublicRooms( GenericRequest *request );
	ByteBuffer &Serialize();
};