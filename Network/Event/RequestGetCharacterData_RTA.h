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

	enum ERROR_CODE {
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

	sptr_generic_response SendCharacterData(sptr_socket socket, sptr_realm_character data);
};

class ResultGetNetCharacterData_RTA : public GenericResponse {
private:
	int32_t m_reply;
	int32_t m_endOfData;
	std::vector< uint8_t > m_chunk;

public:
	ResultGetNetCharacterData_RTA( GenericRequest *request, int32_t reply,
		std::optional< std::vector< uint8_t > > data = std::nullopt,
		std::optional < int32_t > endOfData = std::nullopt );

	void Serialize( ByteBuffer &out ) const;
};