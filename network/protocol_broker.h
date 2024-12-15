#pragma once

// ╔═╗╦ ╦╔═╗╔╦╗╔═╗╦╔═╗╔╗╔╔═╗                   
// ║  ╠═╣╠═╣║║║╠═╝║║ ║║║║╚═╗                   
// ╚═╝╩ ╩╩ ╩╩ ╩╩  ╩╚═╝╝╚╝╚═╝                   
// ╔╗ ╦═╗╔═╗╦╔═╔═╗╦═╗  ╔═╗╦═╗╔═╗╔╦╗╔═╗╔═╗╔═╗╦  
// ╠╩╗╠╦╝║ ║╠╩╗║╣ ╠╦╝  ╠═╝╠╦╝║ ║ ║ ║ ║║  ║ ║║  
// ╚═╝╩╚═╚═╝╩ ╩╚═╝╩╚═  ╩  ╩╚═╚═╝ ╩ ╚═╝╚═╝╚═╝╩═╝

#include "packet.h"
#include "socket.h"

namespace Protocol::Broker
{
	void process_request( sptr_socket socket, sptr_packet r );		// Process incoming packets
	void process_notice( sptr_socket socket, sptr_packet r );		// Process socket notices
}