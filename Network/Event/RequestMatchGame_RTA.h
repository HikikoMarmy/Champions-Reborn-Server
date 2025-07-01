#pragma once

#include <memory>
#include <string>

#include "../GenericNetRequest.h"
#include "../GenericNetResponse.h"

class RequestMatchGame_RTA : public GenericRequest
{
public:
	static std::unique_ptr< RequestMatchGame_RTA > Create()
	{
		return std::make_unique< RequestMatchGame_RTA >();
	}

	sptr_generic_response ProcessRequest( sptr_socket socket, sptr_byte_stream stream ) override;
	void Deserialize( sptr_byte_stream stream ) override;
};

class ResultMatchGame_RTA : public GenericResponse {
private:
	std::string m_userIp;

public:
	ResultMatchGame_RTA( GenericRequest *request, std::string userIp );
	void Serialize( ByteBuffer &out ) const;
};