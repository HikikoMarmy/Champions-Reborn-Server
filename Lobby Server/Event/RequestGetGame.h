#pragma once

class RequestGetGame : public GenericRequest
{
private:
	std::wstring m_sessionId;
	std::wstring m_gameName;

public:
	static std::unique_ptr< RequestGetGame > Create()
	{
		return std::make_unique< RequestGetGame >();
	}
	sptr_generic_response ProcessRequest( sptr_user user, sptr_byte_stream stream ) override;
	void Deserialize( sptr_byte_stream stream ) override;
};

class ResultGetGame : public GenericResponse {
public:
	ResultGetGame( GenericRequest *request );
	ByteStream &Serialize();
};