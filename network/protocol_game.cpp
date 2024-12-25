/*
#include "../global_define.h"

using namespace Protocol::Game;

typedef void( *FunctionPointer )( sptr_client, sptr_packet );
const std::map< uint16_t, FunctionPointer >	GAME_PROTOCOL =
{
	{ 0x0500, &ReqCancelGame },
	{ 0x0800, &ReqCreatePrivateGame },
	{ 0x0A00, &ReqCreatePublicGame },
	{ 0x0D00, &ReqGetRoom },
	{ 0x1600, &ReqLogin },
	{ 0x2400, &ReqTouchSession },
	{ 0x2500, &ReqDoClientDiscovery },
	{ 0x2700, &ReqGetEncryptionKey },
	{ 0x4200, &ReqGetRules },
	{ 0x4400, &ReqUpdateGameData }
};

void Protocol::Game::process_request( sptr_socket socket, sptr_packet request )
{
	if( request->get_command() == 0xFFFF )
	{
		process_notice( socket, request );
	}
	else
	{
		sptr_client client;

		if( false == client_manager->get_client( socket, client ) )
		{
			socket->flag.disconnected = 1;
			socket.reset();
			return;
		}

		auto protocol_iter = GAME_PROTOCOL.find( request->get_command() );
		if( protocol_iter == GAME_PROTOCOL.end() )
		{
			Log::Error( "UNDEFINED PROTOCOL: %02X", request->get_command() );
			logging.packet( request->buffer, false );
			return;
		}

		( *protocol_iter->second )( client, request );
	}
}

void Protocol::Game::process_notice( sptr_socket socket, sptr_packet request )
{
	SocketNotice notice_id = ( SocketNotice )request->read_u32();
	sptr_client client;

	switch( notice_id )
	{
		// A new client connected
		case SocketNotice::NEW_CONNECTION:
		{
			if( false == client_manager->spawn_new( socket, client ) )
			{
				socket->flag.disconnected = 1;
				socket.reset();
				return;
			}
		} break;

		// A client disconnected or a socket error occured
		case SocketNotice::DISCONNECTED:
		{
			if( false == client_manager->get_client( socket, client ) )
			{
				return;
			}

			client_manager->remove_client( client->socket );
		} break;
	}
}

void Protocol::Game::ReqCancelGame( sptr_client client, sptr_packet request )
{
	// TODO: here we would notify everyone else in the room, probably on the UDP socket bound to this room?

	auto res = make_packet( request );
	{
	}
	client->socket->send( res );
}

void Protocol::Game::ReqCreatePrivateGame( sptr_client client, sptr_packet request )
{
	uint32_t total_size = request->read_u32() * 2;
	uint32_t exp_size = request->read_u32();
	request->skip( total_size - 4 );

	std::wstring room_name = request->read_sz_utf16();

	// TODO: Create Discovery Host and Port for the Broker Server

	auto res = make_packet( request );
	{
		res->write_sz_utf8( ServerConfig::Get().service_ip );		// Discovery Host
		res->write_u32( ServerConfig::Get().broker_port );			// Discovery Port
	}
	client->socket->send( res );
}

void Protocol::Game::ReqCreatePublicGame( sptr_client client, sptr_packet request )
{
}

void Protocol::Game::ReqGetRoom( sptr_client client, sptr_packet request )
{
	uint32_t total_size = request->read_u32();
	uint32_t exp_size = request->read_u32();

	//std::array< uint8_t, 16 > game_id = request->read< std::array< uint8_t, 16 > >();
	request->skip( 16 );

	std::wstring room_name = request->read_utf16();

	auto res = make_packet( request );
	{
		res->write_utf16( L"Room Name" );
		res->write_utf16( L"Room Banner" );

		res->write_u32( 4 );	// Num Members

		res->write_utf8( "Name 1" );
		res->write_utf8( "Name 2" );
		res->write_utf8( "Name 3" );
		res->write_utf8( "Name 4" );

		res->write_u32( 4 );	// Num Moderators

		res->write_utf8( "Moderator 1" );
		res->write_utf8( "Moderator 2" );
		res->write_utf8( "Moderator 3" );
		res->write_utf8( "Moderator 4" );
	}
	client->socket->send( res );
}

void Protocol::Game::ReqLogin( sptr_client, sptr_packet request )
{

}

void Protocol::Game::ReqTouchSession( sptr_client client, sptr_packet request )
{
	std::string session_key = request->read_utf8();

	auto res = make_packet( request );
	{
	}
	client->socket->send( res );
}

void Protocol::Game::ReqDoClientDiscovery( sptr_client client, sptr_packet request )
{
	auto res = make_packet( request );
	{
		res->write_sz_utf8( ServerConfig::Get().service_ip );
		res->write_u32( ServerConfig::Get().broker_port );
	}
	client->socket->send( res );
}

void Protocol::Game::ReqGetEncryptionKey( sptr_client client, sptr_packet request )
{
	std::string public_key = request->read_utf8();
	uint32_t unknown = request->read_u32();

	Encryptor &encryptor = client->encryptor;

	auto privateKey = encryptor.generatePrivateSymKey();

	auto res = make_packet( request );
	{
		res->write_u32( 20 ); // Buffer Length (16 + 4)
		res->write_u32( 0 );// encryptor.m_encryptedPrivateKey.length() );
		res->write_utf8( "" );// encryptor.m_encryptedPrivateKey);
	}
	client->socket->send( res );
}

void Protocol::Game::ReqGetRules( sptr_client client, sptr_packet request )
{
	std::wstring message = L"This is a test message sent from the server!\n\nHurray!";

	auto res = make_packet( request );
	{
		res->write_utf16( message );
	}
	client->socket->send( res );
}

void Protocol::Game::ReqUpdateGameData( sptr_client client, sptr_packet request )
{
	uint32_t total_size = request->read_u32() * 2;
	uint32_t exp_size = request->read_u32();
	request->skip( total_size - 4 );

	std::string game_data = request->read_utf8();	// Seems to be one long string "1 / 4: <description> etc etc". Probably fixed length data structure.

	auto res = make_packet( request );
	{

	}
	client->socket->send( res );
}
*/