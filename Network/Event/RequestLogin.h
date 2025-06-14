#pragma once

#include <memory>
#include <string>

#include "../GenericNetRequest.h"
#include "../GenericNetResponse.h"

class RequestLogin : public GenericRequest
{
private:
	enum LOGIN_REPLY {
		SUCCESS = 0,
		FATAL_ERROR,
		ACCOUNT_INVALID = 4,
	};

	std::string m_username;
	std::string m_password;
	std::wstring m_sessionId;

public:
	static std::unique_ptr< RequestLogin > Create()
	{
		return std::make_unique< RequestLogin >();
	}

	sptr_generic_response ProcessRequest( sptr_socket socket, sptr_byte_stream stream ) override;
	void Deserialize( sptr_byte_stream stream ) override;

	sptr_generic_response ProcessLoginCON( sptr_user user );
	sptr_generic_response ProcessLoginRTA( sptr_user user );
};

class ResultLogin : public GenericResponse {
private:
	std::wstring m_sessionId;
	int32_t m_reply;

public:
	ResultLogin( GenericRequest *request, int32_t reply, std::wstring sessionId );
	ByteBuffer &Serialize();
};