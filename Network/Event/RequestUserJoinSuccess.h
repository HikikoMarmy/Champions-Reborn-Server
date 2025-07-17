#pragma once

#include <memory>
#include <string>

#include "../GenericNetRequest.h"
#include "../GenericNetResponse.h"

class RequestUserJoinSuccess : public GenericRequest
{
private:
	std::wstring m_sessionId;
	std::wstring m_ownerSessionId;

	enum ERROR_CODE {
		SUCCESS = 0,
		FATAL_ERROR,
		GENERAL_ERROR,
	};
public:
	static std::unique_ptr< RequestUserJoinSuccess > Create()
	{
		return std::make_unique< RequestUserJoinSuccess >();
	}

	sptr_generic_response ProcessRequest( sptr_socket socket, sptr_byte_stream stream ) override;
	void Deserialize( sptr_byte_stream stream ) override;
};

class ResultUserJoinSuccess : public GenericResponse {
private:
	int32_t m_reply;

public:
	ResultUserJoinSuccess( GenericRequest *request, int32_t reply );
	void Serialize( ByteBuffer &out ) const;
};