#pragma once

class RequestEnterRoom : public GenericRequest
{
public:
	static std::unique_ptr< RequestEnterRoom > Create()
	{
		return std::make_unique< RequestEnterRoom >();
	}

	sptr_generic_response ProcessRequest( sptr_user user, sptr_byte_stream stream ) override;
	void Deserialize( sptr_byte_stream stream ) override;
};

class ResultEnterRoom : public GenericResponse {
public:
	ResultEnterRoom( GenericRequest *request );
	ByteStream &Serialize();
};