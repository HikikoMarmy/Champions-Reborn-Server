#pragma once

#include <memory>
#include <string>

#include "../GenericNetRequest.h"
#include "../GenericNetResponse.h"

class RequestRemoveFriend : public GenericRequest
{
private:
	std::wstring m_sessionId;
	std::wstring m_chatHandle;

	enum ERROR_CODE {
		SUCCESS = 0,
		FATAL_ERROR,
		DATABASE_ERROR = 2,
		FRIEND_IGNORING = 19,
		FRIEND_INVALID = 20,
		FRIEND_DUPLICATE = 21,
	};
public:
	static std::unique_ptr< RequestRemoveFriend > Create()
	{
		return std::make_unique< RequestRemoveFriend >();
	}

	sptr_generic_response ProcessRequest( sptr_socket socket, sptr_byte_stream stream ) override;
	void Deserialize( sptr_byte_stream stream ) override;
};

class ResultRemoveFriend : public GenericResponse {
private:
	int32_t m_reply;

public:
	ResultRemoveFriend( GenericRequest *request, int32_t reply );
	void Serialize( ByteBuffer &out ) const;
};