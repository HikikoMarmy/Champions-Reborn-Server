#pragma once

#include "../GenericNetRequest.h"
#include "../GenericNetResponse.h"

#include "../../Game/RealmCharacter.h"

class RequestSaveCharacter_RTA : public GenericRequest
{
private:
	std::wstring m_sessionId_A;
	std::wstring m_sessionId_B;
	std::vector< uint8_t > m_data;
	uint32_t m_characterId;

	sptr_realm_character m_characterData;
	
	enum CREATE_REPLY {
		SUCCESS = 0,
		FATAL_ERROR,
		GENERAL_ERROR,
	};
public:
	static std::unique_ptr< RequestSaveCharacter_RTA > Create()
	{
		return std::make_unique< RequestSaveCharacter_RTA >();
	}

	sptr_generic_response ProcessRequest( sptr_socket socket, sptr_byte_stream stream ) override;
	void Deserialize( sptr_byte_stream stream ) override;
};

class ResultSaveCharacter_RTA : public GenericResponse {
private:
	int32_t m_reply;

public:
	ResultSaveCharacter_RTA( GenericRequest *request, int32_t reply );
	ByteBuffer &Serialize();
};