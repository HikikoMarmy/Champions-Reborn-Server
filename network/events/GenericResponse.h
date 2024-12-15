#pragma once

#include "../misc/ByteStream.h"
#include "../network/packet.h"

class GenericResponse
{
public:
	uint16_t ProtocolID;
	uint32_t EventID;
	uint32_t State;
	ByteStream stream;

public:
	GenericResponse()
	{
		ProtocolID = 0;
		EventID = 0;
		State = 0;
	}

	~GenericResponse()
	{

	}

	virtual void pack()
	{
	};
};

typedef std::shared_ptr< GenericResponse > sptr_response;