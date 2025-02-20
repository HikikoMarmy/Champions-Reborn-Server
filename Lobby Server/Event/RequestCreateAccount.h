#pragma once

// Account Creation is used in the Network Beta for CoN
// but it isn't used or supported here because retail
// uses "foo" and "bar" to login without user data.

class RequestCreateAccount : public GenericRequest
{
	enum CREATE_ACCOUNT_REPLY {
		SUCCESS = 0,
		ERROR_FATAL,
		ERROR_NOT_EXIST
	};
public:
	static std::unique_ptr< RequestCreateAccount > Create()
	{
		return std::make_unique< RequestCreateAccount >();
	}

	CREATE_ACCOUNT_REPLY m_reply;

	sptr_generic_response ProcessRequest( sptr_user user, sptr_byte_stream stream ) override;
	void Deserialize( sptr_byte_stream stream ) override;
};

class ResultCreateAccount : public GenericResponse {
private:
	std::wstring m_sessionId;
	int32_t m_reply;

public:
	ResultCreateAccount( GenericRequest *request, int32_t reply, std::wstring sessionId );
	ByteStream &Serialize();
};