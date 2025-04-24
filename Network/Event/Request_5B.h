#pragma once

class Request_5B : public GenericRequest
{
public:
	static std::unique_ptr< Request_5B > Create()
	{
		return std::make_unique< Request_5B >();
	}

	sptr_generic_response ProcessRequest( sptr_user user, sptr_byte_stream stream ) override;
	void Deserialize( sptr_byte_stream stream ) override;
};

class Result_5B : public GenericResponse {
public:
	Result_5B( GenericRequest *request );
	ByteStream &Serialize();
};