#pragma once

#include <memory>
#include <string>

#include "../GenericNetRequest.h"
#include "../GenericNetResponse.h"

class RequestCreatePrivateRoom : public GenericRequest
{
private:
	std::wstring m_sessionId;
	std::wstring m_roomName;

public:
	static std::unique_ptr< RequestCreatePrivateRoom > Create()
	{
		return std::make_unique< RequestCreatePrivateRoom >();
	}

	sptr_generic_response ProcessRequest( sptr_socket socket, sptr_byte_stream stream ) override;
	void Deserialize( sptr_byte_stream stream ) override;
};

class ResultCreatePrivateRoom : public GenericResponse {
public:
	ResultCreatePrivateRoom( GenericRequest *request );
	void Serialize( ByteBuffer &out ) const;
};