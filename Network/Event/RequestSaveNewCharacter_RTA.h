#pragma once

class RequestSaveNewCharacter : public GenericRequest
{
public:
	static std::unique_ptr< RequestSaveNewCharacter > Create()
	{
		return std::make_unique< RequestSaveNewCharacter >();
	}

	sptr_generic_response ProcessRequest( sptr_user user, sptr_byte_stream stream ) override;
	void Deserialize( sptr_byte_stream stream ) override;
};

class ResultSaveNewCharacter : public GenericResponse {
public:
	ResultSaveNewCharacter( GenericRequest *request );
	ByteStream &Serialize();
};