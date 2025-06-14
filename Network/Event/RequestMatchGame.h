#pragma once

#include "../GenericNetRequest.h"
#include "../GenericNetResponse.h"

class RequestMatchGame : public GenericRequest
{
private:
	std::wstring m_sessionId;

public:
	static std::unique_ptr< RequestMatchGame > Create()
	{
		return std::make_unique< RequestMatchGame >();
	}

	sptr_generic_response ProcessRequest( sptr_socket socket, sptr_byte_stream stream ) override;
	void Deserialize( sptr_byte_stream stream ) override;
};

class ResultMatchGame : public GenericResponse {
private:
	std::string m_userIp;

public:
	ResultMatchGame( GenericRequest *request, std::string userIp );
	ByteBuffer &Serialize();
};