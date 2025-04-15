#pragma once

class RequestCreatePrivateGame : public GenericRequest
{
private:
	std::wstring m_sessionId;
	std::wstring m_gameInfo;

	enum CREATE_REPLY {
		SUCCESS = 0,
		FATAL_ERROR,
		GENERAL_ERROR,
		GAME_NAME_IN_USE = 38,
		GAME_PENDING = 40,
	};

public:
	static std::unique_ptr< RequestCreatePrivateGame > Create()
	{
		return std::make_unique< RequestCreatePrivateGame >();
	}

	sptr_generic_response ProcessRequest( sptr_user user, sptr_byte_stream stream ) override;
	void Deserialize( sptr_byte_stream stream ) override;
};

class ResultCreatePrivateGame : public GenericResponse {
private:
	int32_t m_reply;
	std::string m_discoveryIp;
	int32_t m_discoveryPort;

public:
	ResultCreatePrivateGame( GenericRequest *request, int32_t reply, std::string discoveryIp = "", int32_t discoveryPort = 0 );
	ByteStream &Serialize();
};