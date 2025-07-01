#pragma once

#include <memory>

#include "../Common/ByteStream.h"

class GenericResponse;
using sptr_generic_response = std::shared_ptr< GenericResponse >;

class RealmUser;
using sptr_user = std::shared_ptr< RealmUser >;

class RealmSocket;
using sptr_socket = std::shared_ptr< RealmSocket >;

class GenericRequest {
public:
	int16_t m_packetId;
	uint32_t m_trackId;

	virtual ~GenericRequest() = default;

	virtual sptr_generic_response ProcessRequest( sptr_socket user, sptr_byte_stream stream )
	{
		throw std::runtime_error( "ProcessRequest not implemented for GenericRequest" );
	}

	virtual sptr_generic_response ProcessRequest( sptr_byte_stream stream )
	{
		throw std::runtime_error( "ProcessRequest not implemented for GenericRequest" );
	}

	void DeserializeHeader( sptr_byte_stream stream )
	{
		m_packetId = stream->read_u16();
		m_trackId = stream->read_u32();
		auto version = stream->read_u32();	// 2 for CON and 6 for RTA
	};

	virtual void Deserialize( sptr_byte_stream stream ) = 0;
};

using sptr_generic_request = std::shared_ptr< GenericRequest >;

