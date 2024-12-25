#pragma once

class RequestTouchSession : public GenericRequest
{
private:
	std::wstring m_sessionId;

public:
	static std::unique_ptr< RequestTouchSession > Create()
	{
		return std::make_unique< RequestTouchSession >();
	}
	sptr_generic_response ProcessRequest( sptr_tcp_socket socket, sptr_byte_stream stream ) override;
	void Deserialize( sptr_tcp_socket socket, sptr_byte_stream stream ) override;
};

class ResultTouchSession : public GenericResponse {
public:
	ResultTouchSession( GenericRequest *request );
	ByteStream &Serialize();
};