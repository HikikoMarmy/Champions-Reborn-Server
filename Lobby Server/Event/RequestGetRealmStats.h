#pragma once

class RequestGetRealmStats : public GenericRequest
{
private:

public:
	static std::unique_ptr< RequestGetRealmStats > Create()
	{
		return std::make_unique< RequestGetRealmStats >();
	}
	sptr_generic_response ProcessRequest( sptr_tcp_socket socket, sptr_byte_stream stream ) override;
	void Deserialize( sptr_tcp_socket socket, sptr_byte_stream stream ) override;
};

class ResultGetRealmStats : public GenericResponse {
public:
	ResultGetRealmStats( GenericRequest *request );
	ByteStream &Serialize();
};