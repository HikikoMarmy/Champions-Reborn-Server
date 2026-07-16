#pragma once

#include <memory>
#include <string>

#include "../GenericNetRequest.hpp"
#include "../GenericNetResponse.hpp"

class RequestDeleteCharacter_RTA : public GenericRequest {
private:
	std::wstring m_sessionId;
	std::int32_t m_characterId;

	enum REPLY {
		SUCCESS = 0,
		TIMEOUT,
		NOT_FOUND
	};

public:
	static std::unique_ptr< RequestDeleteCharacter_RTA > Create()
	{
		return std::make_unique< RequestDeleteCharacter_RTA >();
	}
	sptr_generic_response ProcessRequest( sptr_socket socket, sptr_byte_stream stream ) override;
	void Deserialize( sptr_byte_stream stream ) override;
};

class ResultDeleteCharacter_RTA : public GenericResponse {
private:
	int32_t m_reply;
	int32_t m_gameId;

	std::wstring m_discoveryAddr;
	std::wstring m_localAddr;
	int32_t m_port;

public:
	ResultDeleteCharacter_RTA( GenericRequest *request, int32_t reply );
	void Serialize( ByteBuffer &out ) const;
};