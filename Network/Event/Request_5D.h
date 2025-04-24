#pragma once

class Request_5D : public GenericRequest
{
public:
	static std::unique_ptr< Request_5D > Create()
	{
		return std::make_unique< Request_5D >();
	}

	sptr_generic_response ProcessRequest( sptr_user user, sptr_byte_stream stream ) override;
	void Deserialize( sptr_byte_stream stream ) override;
};

class Result_5D : public GenericResponse {
public:
	Result_5D( GenericRequest *request );
	ByteStream &Serialize();
};