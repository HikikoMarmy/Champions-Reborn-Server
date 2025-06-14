#pragma once

#include <memory>
#include <string>

#include "../GenericNetRequest.h"
#include "../GenericNetResponse.h"

class RequestGetGame_RTA : public GenericRequest {
private:
	std::wstring m_sessionId;
	std::string m_localAddr;

	uint32_t m_gameId;

	enum REPLY {
		SUCCESS = 0,
		TIMEOUT,
		NOT_FOUND
	};

public:
	static std::unique_ptr< RequestGetGame_RTA > Create()
	{
		return std::make_unique< RequestGetGame_RTA >();
	}
	sptr_generic_response ProcessRequest( sptr_socket socket, sptr_byte_stream stream ) override;
	void Deserialize( sptr_byte_stream stream ) override;
};

class ResultGetGame_RTA : public GenericResponse {
private:
	int32_t m_reply;
	std::string m_discoveryIp;
	int32_t m_discoveryPort;

public:
	ResultGetGame_RTA( GenericRequest *request, int32_t reply, std::string discoveryIp = "", int32_t discoveryPort = 0);
	ByteBuffer &Serialize();
};