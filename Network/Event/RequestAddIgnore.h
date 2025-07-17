#pragma once

#include <memory>
#include <string>

#include "../GenericNetRequest.h"
#include "../GenericNetResponse.h"

class RequestAddIgnore : public GenericRequest
{
private:
	std::wstring m_sessionId;
	std::wstring m_chatHandle;

	enum ERROR_CODE {
		SUCCESS = 0,
		FATAL_ERROR,
		DATABASE_ERROR = 2,
		IGNORE_INVALID = 20,
		IGNORE_FRIEND = 24,
		IGNORE_DUPLICATE = 25,
	};

public:
	static std::unique_ptr< RequestAddIgnore > Create()
	{
		return std::make_unique< RequestAddIgnore >();
	}

	sptr_generic_response ProcessRequest( sptr_socket socket, sptr_byte_stream stream ) override;
	void Deserialize( sptr_byte_stream stream ) override;
};

class ResultAddIgnore : public GenericResponse {
private:
	int32_t m_reply;

public:
	ResultAddIgnore( GenericRequest *request, int32_t reply );
	void Serialize( ByteBuffer &out ) const;
};