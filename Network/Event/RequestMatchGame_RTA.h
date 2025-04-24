#pragma once

class RequestMatchGame_RTA : public GenericRequest
{
public:
	static std::unique_ptr< RequestMatchGame_RTA > Create()
	{
		return std::make_unique< RequestMatchGame_RTA >();
	}

	sptr_generic_response ProcessRequest( sptr_user user, sptr_byte_stream stream ) override;
	void Deserialize( sptr_byte_stream stream ) override;
};

class ResultMatchGame_RTA : public GenericResponse {
public:
	ResultMatchGame_RTA( GenericRequest *request );
	ByteStream &Serialize();
};