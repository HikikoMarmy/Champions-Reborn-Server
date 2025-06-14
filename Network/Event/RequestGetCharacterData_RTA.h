#pragma once

#include <memory>
#include <string>

#include "../GenericNetRequest.h"
#include "../GenericNetResponse.h"

#include "../../Game/RealmCharacter.h"

class RequestGetNetCharacterData_RTA : public GenericRequest
{
private:
	int32_t m_characterId;

	enum CREATE_REPLY {
		SUCCESS = 0,
		FATAL_ERROR,
		GENERAL_ERROR,
	};

public:
	static std::unique_ptr< RequestGetNetCharacterData_RTA > Create()
	{
		return std::make_unique< RequestGetNetCharacterData_RTA >();
	}

	sptr_generic_response ProcessRequest( sptr_socket socket, sptr_byte_stream stream ) override;
	void Deserialize( sptr_byte_stream stream ) override;
};

class ResultGetNetCharacterData_RTA : public GenericResponse {
private:
	int32_t m_reply;
	sptr_realm_character m_data;

public:
	ResultGetNetCharacterData_RTA( GenericRequest *request, int32_t reply, sptr_realm_character data );
	ByteBuffer &Serialize();
};