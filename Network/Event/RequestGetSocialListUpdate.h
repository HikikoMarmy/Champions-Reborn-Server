#pragma once

#include <memory>
#include <string>

#include "../GenericNetRequest.h"
#include "../GenericNetResponse.h"
#include "../../Common/ForwardDecl.h"

class RequestGetSocialListUpdate : public GenericRequest
{
public:
	static std::unique_ptr< RequestGetSocialListUpdate > Create()
	{
		return std::make_unique< RequestGetSocialListUpdate >();
	}

	sptr_generic_response ProcessRequest( sptr_socket socket, sptr_byte_stream stream ) override;
	void Deserialize( sptr_byte_stream stream ) override;
};

class ResultGetSocialListUpdate : public GenericResponse {
private:
	int32_t m_reply;
	sptr_user m_user;

public:
	ResultGetSocialListUpdate( GenericRequest *request, sptr_user user = nullptr );
	void Serialize( ByteBuffer &out ) const;
};