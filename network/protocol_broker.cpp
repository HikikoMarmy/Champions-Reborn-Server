
#include "../global_define.h"

#include "protocol_broker.h"

using namespace Protocol::Broker;

typedef void( *FunctionPointer )( sptr_socket, sptr_packet );
const std::map< uint16_t, FunctionPointer >	BROKER_PROTOCOL =
{

};

void Protocol::Broker::process_request( sptr_socket socket, sptr_packet request )
{
	if( request->get_command() == 0xFFFF )
	{
		process_notice( socket, request );
		return;
	}
	
	auto protocol_iter = BROKER_PROTOCOL.find( request->get_command() );
	if( protocol_iter == BROKER_PROTOCOL.end() )
	{
		logging.error( "UNDEFINED PROTOCOL: %02X", request->get_command() );
		logging.packet( request->buffer, false );
		return;
	}

	( *protocol_iter->second )( socket, request );
}

void Protocol::Broker::process_notice( sptr_socket s, sptr_packet r )
{
	SocketNotice notice_id = ( SocketNotice )r->read_u32();
	sptr_client client;

	switch( notice_id )
	{
		case SocketNotice::NEW_CONNECTION:
		{
		} break;

		case SocketNotice::DISCONNECTED:
		{
		} break;
	}
}