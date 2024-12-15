#pragma once
/*
#include "../network/socket.h"
#include "../network/packet.h"
#include "../misc/ByteStream.h"

class GenericRequest
{
public:
	uint16_t ProtocolID;
	uint32_t EventID;
	uint32_t State;
	ByteStream stream;

public:
	GenericRequest()
	{
		ProtocolID = 0;
		EventID = 0;
		State = 0;
	}

	~GenericRequest()
	{
	};

	GenericRequest( sptr_packet request )
	{
		ProtocolID = request->read_u16();
		EventID = request->read_u32();
		State = request->read_u32();

		stream = ByteStream( request->raw );
	}

	GenericRequest( ByteStream stream )
	{
		ProtocolID = stream.read_u16();
		EventID = stream.read_u32();
		State = stream.read_u32();

		this->stream = stream;
	}

	GenericRequest( const GenericRequest & ) = delete;
	GenericRequest &operator=( const GenericRequest & ) = delete;

	virtual void unpack() = 0;
	virtual void process( sptr_socket s, sptr_packet r ) = 0;
};

typedef std::shared_ptr< GenericRequest > sptr_request;
*/