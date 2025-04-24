#pragma once

class RequestCreatePrivateRoom : public GenericRequest
{
public:
	static std::unique_ptr< RequestCreatePrivateRoom > Create()
	{
		return std::make_unique< RequestCreatePrivateRoom >();
	}

	sptr_generic_response ProcessRequest( sptr_user user, sptr_byte_stream stream ) override;
	void Deserialize( sptr_byte_stream stream ) override;
};

class ResultCreatePrivateRoom : public GenericResponse {
public:
	ResultCreatePrivateRoom( GenericRequest *request );
	ByteStream &Serialize();
};