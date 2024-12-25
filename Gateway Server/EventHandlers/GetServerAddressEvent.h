#pragma once

class RequestGetServerAddress : public GenericRequest {
public:
	static std::unique_ptr< RequestGetServerAddress > Create()
	{
		return std::make_unique< RequestGetServerAddress >();
	}
	sptr_generic_response ProcessRequest( sptr_tcp_socket socket, sptr_byte_stream stream ) override;
	void Deserialize( sptr_tcp_socket socket, sptr_byte_stream stream ) override;
};

class ResultGetServerAddress : public GenericResponse {
public:
	std::string m_ip;
	int32_t		m_port;

	ResultGetServerAddress( GenericRequest *request, std::string ip, int32_t port );
	ByteStream &Serialize();
};