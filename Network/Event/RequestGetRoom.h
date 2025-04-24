#pragma once

class RequestGetRoom : public GenericRequest {
public:
	static std::unique_ptr< RequestGetRoom > Create()
	{
		return std::make_unique< RequestGetRoom >();
	}

	sptr_generic_response ProcessRequest( sptr_user user, sptr_byte_stream stream ) override;
	void Deserialize( sptr_byte_stream stream ) override;
};

class ResultGetRoom : public GenericResponse {
public:
	ResultGetRoom( GenericRequest *request );
	ByteStream &Serialize();
};