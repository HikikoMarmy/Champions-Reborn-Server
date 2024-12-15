#pragma once

// ╔═╗╦ ╦╔═╗╔╦╗╔═╗╦╔═╗╔╗╔╔═╗             
// ║  ╠═╣╠═╣║║║╠═╝║║ ║║║║╚═╗             
// ╚═╝╩ ╩╩ ╩╩ ╩╩  ╩╚═╝╝╚╝╚═╝             
// ╔═╗╔═╗╔╦╗╔═╗  ╔═╗╦═╗╔═╗╔╦╗╔═╗╔═╗╔═╗╦  
// ║ ╦╠═╣║║║║╣   ╠═╝╠╦╝║ ║ ║ ║ ║║  ║ ║║  
// ╚═╝╩ ╩╩ ╩╚═╝  ╩  ╩╚═╚═╝ ╩ ╚═╝╚═╝╚═╝╩═╝

#include "packet.h"
#include "socket.h"

namespace Protocol::Game
{
	void process_request( sptr_socket s, sptr_packet r );	// Process incoming packets
	void process_notice( sptr_socket s, sptr_packet r );	// Process socket notices

	void ReqCancelGame( sptr_client client, sptr_packet request );			// 0500
	void ReqCreatePrivateGame( sptr_client client, sptr_packet request );	// 0800
	void ReqCreatePublicGame( sptr_client client, sptr_packet request );	// 0A00
	void ReqGetRoom( sptr_client client, sptr_packet request );				// 0D00
	void ReqLogin( sptr_client client, sptr_packet request );				// 1600
	void ReqTouchSession( sptr_client client, sptr_packet request );		// 2400
	void ReqDoClientDiscovery( sptr_client client, sptr_packet request );	// 2500
	void ReqGetEncryptionKey( sptr_client client, sptr_packet request );	// 2700
	void ReqGetRules( sptr_client client, sptr_packet request );			// 4200
	void ReqUpdateGameData( sptr_client client, sptr_packet request );		// 4400
}