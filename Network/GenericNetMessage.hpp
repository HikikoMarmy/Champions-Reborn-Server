#pragma once

#include "../misc/ByteStream.h"

class GenericMessage
{
public:
	uint16_t m_packetId;
	ByteStream m_stream;

	GenericMessage( uint16_t packetId ) : m_packetId( packetId )
	{
	}

	virtual ~GenericMessage() = default;
	virtual ByteStream& Serialize() = 0;
};

typedef std::shared_ptr< GenericMessage > sptr_generic_message;