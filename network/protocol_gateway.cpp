
#include "../global_define.h"

#include "protocol_gateway.h"
#include "events/Gateway/ReqGetServerAddress.h"

using namespace Protocol::Gateway;

typedef void( *FunctionPointer )( sptr_socket, sptr_packet );
const std::map< uint16_t, FunctionPointer >	GATEWAY_PROTOCOL =
{
	{ 0x4300, &ReqGetServerAddress },
};

void Protocol::Gateway::process_request( sptr_socket socket, sptr_packet request )
{
	if( request->get_command() == 0xFFFF )
	{
		process_notice( socket, request );
		return;
	}
	
	auto protocol_iter = GATEWAY_PROTOCOL.find( request->get_command() );
	if( protocol_iter == GATEWAY_PROTOCOL.end() )
	{
		logging.error( "UNDEFINED PROTOCOL: %02X", request->get_command() );
		logging.packet( request->buffer, false );
		return;
	}

	( *protocol_iter->second )( socket, request );
}

void Protocol::Gateway::process_notice( sptr_socket s, sptr_packet r )
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

void Protocol::Gateway::ReqGetServerAddress( sptr_socket socket, sptr_packet request )
{
	auto res = std::make_shared< PacketBuffer >( 0x4300, request->get_event_id(), 64 );
	{
		res->write_sz_utf8( ServerConfig::Get().service_ip );
		res->write_u32( ServerConfig::Get().session_port );
	}
	socket->send( res );
}
