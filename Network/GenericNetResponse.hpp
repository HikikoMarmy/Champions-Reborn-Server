#pragma once

#include "GenericNetRequest.hpp"
#include "../misc/ByteStream.h"

class GenericResponse
{
public:
	uint16_t m_packetId;
	uint32_t m_requestId;
	ByteStream m_stream;

	GenericResponse( GenericRequest &request ) : m_packetId( request.m_packetId ), m_requestId( request.m_requestId )
	{
	}

	virtual ~GenericResponse() = default;
	virtual ByteStream& Serialize() = 0;
};

typedef std::shared_ptr< GenericResponse > sptr_generic_response;