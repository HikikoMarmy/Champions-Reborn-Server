#pragma once

// Account Login is used in the Network Beta for CoN.
// In the retail version, the game simply logs in with
// "foo" and "bar" as the username and password.
//
// A unique Session ID is generated and assigned to the player.

class RequestLogin : public GenericRequest
{
private:
	enum LOGIN_REPLY {
		SUCCESS = 0,
		FATAL_ERROR,
		NOT_EXIST,
	};

	std::wstring m_username;
	std::wstring m_password;
	std::wstring m_sessionId;

public:
	static std::unique_ptr< RequestLogin > Create()
	{
		return std::make_unique< RequestLogin >();
	}

	sptr_generic_response ProcessRequest( sptr_tcp_socket socket, sptr_byte_stream stream ) override;
	void Deserialize( sptr_tcp_socket socket, sptr_byte_stream stream ) override;
};

class ResultLogin : public GenericResponse {
private:
	std::wstring m_sessionId;
	int32_t m_reply;

public:
	ResultLogin( GenericRequest *request, int32_t reply, std::wstring sessionId );
	ByteStream &Serialize();
};