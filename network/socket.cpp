
#include "../global_define.h"
#include "socket.h"

CRealmSocket::CRealmSocket()
{
	fd = INVALID_SOCKET;

	memset( &local_address, 0, sizeof( local_address ) );
	memset( &remote_address, 0, sizeof( remote_address ) );
	port = 0;

	type = RealmSocketType::TCP;
	channel = RealmChannelType::INVALID;

	flag.disconnected = 0;
	flag.is_listener = 0;
	flag.want_more_read_data = 0;
	flag.want_more_write_data = 0;

	last_write_position = 0;

	latency = 0;
	last_recv_time = 0.0;
	last_send_time = 0.0;
}

CRealmSocket::~CRealmSocket()
{
	if( INVALID_SOCKET != fd )
	{
		closesocket( fd );
	}

	fd = INVALID_SOCKET;

	memset( &local_address, 0, sizeof( local_address ) );
	memset( &remote_address, 0, sizeof( remote_address ) );
	port = 0;

	type = RealmSocketType::TCP;
	channel = RealmChannelType::INVALID;

	flag.disconnected = 0;
	flag.is_listener = 0;
	flag.want_more_read_data = 0;
	flag.want_more_write_data = 0;

	last_write_position = 0;

	latency = 0;
	last_recv_time = 0.0;
	last_send_time = 0.0;
}

void CRealmSocket::send( const sptr_packet p )
{
	// TODO: UDP sockets probably need to be handled differently
	// 
	// Swap the packet size to network byte order
	*( uint32_t * )&p->buffer[ 0 ] = Math::swap_endian( p->write_position );

	logging.packet( p->buffer, true );
	std::lock_guard< std::mutex > lock( write_mutex );
	write_queue.push_back( p );
}