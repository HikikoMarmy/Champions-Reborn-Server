#pragma once

#include "../Network/RealmSocket.h"
#include "../misc/ByteStream.h"

class GenericResponse;
typedef std::shared_ptr< GenericResponse > sptr_generic_response;

class GenericRequest
{
public:
	int16_t m_packetId;
	uint32_t m_requestId;

	virtual ~GenericRequest() = default;

	virtual sptr_generic_response ProcessRequest(sptr_user user, sptr_byte_stream stream)
	{
		return nullptr;
	};

	virtual sptr_generic_response ProcessRequest( sptr_byte_stream stream )
	{
		return nullptr;
	};

	void DeserializeHeader( sptr_byte_stream stream )
	{
		m_packetId = stream->read_u16();
		m_requestId = stream->read_u32();
		auto ver = stream->read_u32();	// Called Version in the game, but is always 2 for CON and RTA
	};
	virtual void Deserialize( sptr_byte_stream stream ) = 0;
};

typedef std::shared_ptr< GenericRequest > sptr_generic_request;

