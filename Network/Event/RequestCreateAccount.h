#pragma once

#include <memory>
#include <string>

#include "../GenericNetRequest.h"
#include "../GenericNetResponse.h"

class RequestCreateAccount : public GenericRequest
{
private:
	enum CREATE_ACCOUNT_REPLY {
		SUCCESS = 0,
		ERROR_FATAL,
		ERROR_NOT_EXIST
	};

	std::wstring m_username;
	std::wstring m_password;
	std::wstring m_emailAddress;
	std::wstring m_dateOfBirth;
	std::wstring m_chatHandle;

	bool VerifyUserData();

public:
	static std::unique_ptr< RequestCreateAccount > Create()
	{
		return std::make_unique< RequestCreateAccount >();
	}

	CREATE_ACCOUNT_REPLY m_reply;

	sptr_generic_response ProcessRequest( sptr_socket socket, sptr_byte_stream stream ) override;
	void Deserialize( sptr_byte_stream stream ) override;
};

class ResultCreateAccount : public GenericResponse {
private:
	std::wstring m_sessionId;
	int32_t m_reply;

public:
	ResultCreateAccount( GenericRequest *request, int32_t reply, std::wstring sessionId );
	void Serialize( ByteBuffer &out ) const;
};