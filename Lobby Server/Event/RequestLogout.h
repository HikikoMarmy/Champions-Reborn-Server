#pragma once

class RequestLogout : public GenericRequest
{
private:
	std::wstring m_sessionId;

public:
	static std::unique_ptr< RequestLogout > Create()
	{
		return std::make_unique< RequestLogout >();
	}
	sptr_generic_response ProcessRequest( sptr_user user, sptr_byte_stream stream ) override;
	void Deserialize( sptr_byte_stream stream ) override;
};

class ResultLogout : public GenericResponse {
private:
	int32_t m_reply;
public:
	ResultLogout( GenericRequest *request, int32_t reply );
	ByteStream &Serialize();
};