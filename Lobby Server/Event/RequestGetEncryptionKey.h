#pragma once

class RequestGetEncryptionKey : public GenericRequest
{
public:
	static std::unique_ptr< RequestGetEncryptionKey > Create()
	{
		return std::make_unique< RequestGetEncryptionKey >();
	}

	sptr_generic_response ProcessRequest( sptr_tcp_socket socket, sptr_byte_stream stream ) override;
	void Deserialize( sptr_tcp_socket socket, sptr_byte_stream stream ) override;
};

class ResultGetEncryptionKey : public GenericResponse {
public:
	std::vector< uint8_t > m_symKey;

	ResultGetEncryptionKey( GenericRequest *request );
	ByteStream &Serialize();
};