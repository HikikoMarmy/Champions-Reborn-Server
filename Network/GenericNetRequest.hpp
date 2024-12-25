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

	virtual sptr_generic_response ProcessRequest( sptr_tcp_socket socket, sptr_byte_stream stream ) = 0;
	void DeserializeHeader( sptr_byte_stream stream )
	{
		m_packetId = stream->read_u16();
		m_requestId = stream->read_u32();
		auto _ = stream->read_u32();	// Always 2 from client.
	};
	virtual void Deserialize( sptr_tcp_socket socket, sptr_byte_stream stream ) = 0;
};

typedef std::shared_ptr< GenericRequest > sptr_generic_request;

