#pragma once

#include <memory>
#include <string>

#include "../GenericNetRequest.h"
#include "../GenericNetResponse.h"

class RequestStartGame : public GenericRequest
{
private:
	std::vector< uint8_t > m_data;

	enum ERROR_CODE {
		SUCCESS = 0,
		FATAL_ERROR,
		GENERAL_ERROR,
	};
public:
	static std::unique_ptr< RequestStartGame > Create()
	{
		return std::make_unique< RequestStartGame >();
	}

	sptr_generic_response ProcessRequest( sptr_socket socket, sptr_byte_stream stream ) override;
	void Deserialize( sptr_byte_stream stream ) override;
};

class ResultStartGame : public GenericResponse {
private:
	int32_t m_reply;

public:
	ResultStartGame( GenericRequest *request, int32_t reply );
	void Serialize( ByteBuffer &out ) const;
};