#pragma once

#include <memory>
#include <string>

#include "../GenericNetRequest.h"
#include "../GenericNetResponse.h"

class RequestDoClientDiscovery_RTA : public GenericRequest {
private:
	std::wstring m_sessionId;
	std::wstring m_localAddr;
	int32_t m_localPort;

	uint32_t m_gameId;

	enum REPLY {
		SUCCESS = 0,
		TIMEOUT,
		NOT_FOUND
	};

public:
	static std::unique_ptr< RequestDoClientDiscovery_RTA > Create()
	{
		return std::make_unique< RequestDoClientDiscovery_RTA >();
	}
	sptr_generic_response ProcessRequest( sptr_socket socket, sptr_byte_stream stream ) override;
	void Deserialize( sptr_byte_stream stream ) override;
};

class ResultDoClientDiscovery_RTA : public GenericResponse {
private:
	int32_t m_reply;
	std::string m_discoveryIp;
	int32_t m_discoveryPort;

public:
	ResultDoClientDiscovery_RTA( GenericRequest *request, int32_t reply, std::string discoveryIp = "", int32_t discoveryPort = 0);
	void Serialize( ByteBuffer &out ) const;
};