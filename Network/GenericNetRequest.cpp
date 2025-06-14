#include "GenericNetRequest.h"

sptr_generic_response GenericRequest::ProcessRequest( sptr_socket socket, sptr_byte_stream stream )
{
	throw std::runtime_error( "ProcessRequest not implemented for GenericRequest" );
}

sptr_generic_response GenericRequest::ProcessRequest( sptr_byte_stream stream )
{
	throw std::runtime_error( "ProcessRequest not implemented for GenericRequest" );
}

void GenericRequest::DeserializeHeader( sptr_byte_stream stream )
{
	m_packetId = stream->read_u16();
	m_trackId = stream->read_u32();
	auto version = stream->read_u32();	// 2 for CON and 6 for RTA
};