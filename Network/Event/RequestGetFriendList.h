#pragma once

#include <memory>
#include <string>

#include "../GenericNetRequest.h"
#include "../GenericNetResponse.h"

class RequestGetFriendList : public GenericRequest
{
public:
	static std::unique_ptr< RequestGetFriendList > Create()
	{
		return std::make_unique< RequestGetFriendList >();
	}

	sptr_generic_response ProcessRequest( sptr_socket socket, sptr_byte_stream stream ) override;
	void Deserialize( sptr_byte_stream stream ) override;
};

class ResultGetFriendList : public GenericResponse {
public:
	ResultGetFriendList( GenericRequest *request );
	void Serialize( ByteBuffer &out ) const;
};