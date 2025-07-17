#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>

#include "../GenericNetRequest.h"
#include "../GenericNetResponse.h"
#include "../../Game/RealmCharacterMetaKV.h"

class RequestGetNetCharacterList_RTA : public GenericRequest
{
private:
	enum ERROR_CODE {
		SUCCESS = 0,
		FATAL_ERROR,
		GENERAL_ERROR,
	};

public:
	static std::unique_ptr< RequestGetNetCharacterList_RTA > Create()
	{
		return std::make_unique< RequestGetNetCharacterList_RTA >();
	}

	sptr_generic_response ProcessRequest( sptr_socket socket, sptr_byte_stream stream ) override;
	void Deserialize( sptr_byte_stream stream ) override;
};

class ResultGetNetCharacterList_RTA : public GenericResponse {
private:
	int32_t m_reply;
	std::map< uint32_t, CharacterSlotData > m_characterList;

public:
	ResultGetNetCharacterList_RTA( GenericRequest *request, int32_t reply, std::optional< std::map< uint32_t, CharacterSlotData > > list );
	void Serialize( ByteBuffer &out ) const;
};