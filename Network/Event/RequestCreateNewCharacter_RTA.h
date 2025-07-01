#pragma once

#include "../GenericNetRequest.h"
#include "../GenericNetResponse.h"

#include "../../Game/RealmCharacter.h"

class RequestCreateNewCharacter_RTA : public GenericRequest
{
private:
	std::wstring m_sessionId;
	CharacterSlotData m_metaData;
	std::vector< uint8_t > m_characterData;
	
	enum CREATE_REPLY {
		SUCCESS = 0,
		FATAL_ERROR,
		GENERAL_ERROR,
	};

public:
	static std::unique_ptr< RequestCreateNewCharacter_RTA > Create()
	{
		return std::make_unique< RequestCreateNewCharacter_RTA >();
	}

	sptr_generic_response ProcessRequest( sptr_socket socket, sptr_byte_stream stream ) override;
	void Deserialize( sptr_byte_stream stream ) override;
};

class ResultCreateNewCharacter_RTA : public GenericResponse {
private:
	int32_t m_reply;

public:
	ResultCreateNewCharacter_RTA( GenericRequest *request, int32_t reply );
	void Serialize( ByteBuffer &out ) const;
};