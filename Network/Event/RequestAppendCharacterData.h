#pragma once

#include <memory>
#include <string>

#include "../GenericNetRequest.h"
#include "../GenericNetResponse.h"

class RequestAppendCharacterData : public GenericRequest
{
private:
	std::vector< uint8_t > m_data;
	int32_t m_endOfData;

	enum ERROR_CODE {
		SUCCESS = 0,
		FATAL_ERROR,
		GENERAL_ERROR,
	};
public:
	static std::unique_ptr< RequestAppendCharacterData > Create()
	{
		return std::make_unique< RequestAppendCharacterData >();
	}

	sptr_generic_response ProcessRequest( sptr_socket socket, sptr_byte_stream stream ) override;
	void Deserialize( sptr_byte_stream stream ) override;
};

class ResultAppendCharacterData : public GenericResponse {
private:
	int32_t m_reply;

public:
	ResultAppendCharacterData( GenericRequest *request, int32_t reply );
	void Serialize( ByteBuffer &out ) const;
};