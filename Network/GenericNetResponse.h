#pragma once

#include "GenericNetRequest.h"
#include "../Common/ByteStream.h"

class GenericResponse
{
public:
	uint16_t m_packetId;
	uint32_t m_trackId;
	ByteBuffer m_stream;

	GenericResponse( const GenericRequest &request )
	{
		m_packetId = request.m_packetId;
		m_trackId = request.m_trackId;
	}

	virtual ~GenericResponse() = default;
	virtual void Serialize( ByteBuffer& out ) const = 0;
};

using sptr_generic_response = std::shared_ptr< GenericResponse >;