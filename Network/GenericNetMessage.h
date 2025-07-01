#pragma once

#include "../Common/ByteStream.h"

class GenericMessage
{
public:
	uint16_t m_packetId;
	ByteBuffer m_stream;

	GenericMessage( uint16_t packetId ) : m_packetId( packetId )
	{
	}

	virtual ~GenericMessage() = default;
	virtual void Serialize( ByteBuffer &out ) const = 0;
};

typedef std::shared_ptr< GenericMessage > sptr_generic_message;