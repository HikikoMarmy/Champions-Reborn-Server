#pragma once

#include <memory>
#include <string>

#include "../GenericNetRequest.h"
#include "../GenericNetResponse.h"

class RequestGetRealmStats : public GenericRequest
{
public:
	static std::unique_ptr< RequestGetRealmStats > Create()
	{
		return std::make_unique< RequestGetRealmStats >();
	}
	sptr_generic_response ProcessRequest( sptr_socket socket, sptr_byte_stream stream ) override;
	void Deserialize( sptr_byte_stream stream ) override;
};

class ResultGetRealmStats : public GenericResponse {
public:
	ResultGetRealmStats( GenericRequest *request );
	void Serialize( ByteBuffer &out ) const;
};