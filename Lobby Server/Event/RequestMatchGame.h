#pragma once

class RequestMatchGame : public GenericRequest
{
private:
	std::wstring m_sessionId;

public:
	static std::unique_ptr< RequestMatchGame > Create()
	{
		return std::make_unique< RequestMatchGame >();
	}
	sptr_generic_response ProcessRequest( sptr_tcp_socket socket, sptr_byte_stream stream ) override;
	void Deserialize( sptr_tcp_socket socket, sptr_byte_stream stream ) override;
};

class ResultMatchGame : public GenericResponse {
public:
	ResultMatchGame( GenericRequest *request );
	ByteStream &Serialize();
};