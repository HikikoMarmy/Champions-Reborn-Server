#pragma once

#include <memory>
#include <string>

#include "../GenericNetRequest.h"
#include "../GenericNetResponse.h"

class RequestGetEncryptionKey : public GenericRequest
{
public:
	static std::unique_ptr< RequestGetEncryptionKey > Create()
	{
		return std::make_unique< RequestGetEncryptionKey >();
	}

	sptr_generic_response ProcessRequest( sptr_socket socket, sptr_byte_stream stream ) override;
	void Deserialize( sptr_byte_stream stream ) override;
};

class ResultGetEncryptionKey : public GenericResponse {
public:
	std::vector< uint8_t > m_symKey;

	ResultGetEncryptionKey( GenericRequest *request );
	void Serialize( ByteBuffer &out ) const;
};