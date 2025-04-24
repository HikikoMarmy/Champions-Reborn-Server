#pragma once

class RequestGetGame : public GenericRequest {
private:
	std::wstring m_sessionId;
	std::wstring m_gameName;

	enum REPLY {
		SUCCESS = 0,
		TIMEOUT,
		NOT_FOUND
	};

public:
	static std::unique_ptr< RequestGetGame > Create()
	{
		return std::make_unique< RequestGetGame >();
	}
	sptr_generic_response ProcessRequest( sptr_user user, sptr_byte_stream stream ) override;
	void Deserialize( sptr_byte_stream stream ) override;
};

class ResultGetGame : public GenericResponse {
private:
	int32_t m_reply;
	int32_t m_gameId;

public:
	ResultGetGame( GenericRequest *request, int32_t reply, int32_t gameId = 0 );
	ByteStream &Serialize();
};