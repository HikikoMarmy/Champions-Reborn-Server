
#include "../global_define.h"
#include "socket_manager.h"

CSocketManager::CSocketManager()
{
	run_worker = true;
	wait_end = true;
	max_fd = 0;

	dirty_read_buffer.resize( 0xFFFF );
	socket_list.clear();
}

CSocketManager::~CSocketManager()
{
	run_worker = false;
	max_fd = 0;

	while( wait_end )
	{
		std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
	}

	socket_list.clear();
}

bool CSocketManager::initialize()
{
	WORD wVersionRequest = MAKEWORD( 2, 2 );
	WSADATA wsaData;

	if( WSAStartup( wVersionRequest, &wsaData ) != 0 )
		return false;

	net_time.Start();

	auto &config = ServerConfig::Get();

	open_tcp_listener( config.service_ip, config.gateway_port, RealmChannelType::GATEWAY );
	open_tcp_listener( config.service_ip, config.session_port, RealmChannelType::GAME );
	open_udp_listener( config.service_ip, config.broker_port, RealmChannelType::DISCOVERY );

	// Start network thread
	std::thread socket_thread( &CSocketManager::net_worker, this );
	socket_thread.detach();

	return true;
}

bool CSocketManager::open_tcp_listener( std::string ip, uint16_t port, RealmChannelType channel )
{
	struct sockaddr_in addr;
	auto socket = std::make_shared< CRealmSocket >();

	if( INVALID_SOCKET == ( socket->fd = ::socket( PF_INET, SOCK_STREAM, IPPROTO_TCP ) ) )
	{
		return false;
	}

	memset( &addr, 0, sizeof( addr ) );
	addr.sin_family = AF_INET;
	addr.sin_port = htons( port );
	addr.sin_addr.s_addr = ( "0.0.0.0" != ip )
		? inet_addr( ip.c_str() )
		: htonl( INADDR_ANY );

	if( bind( socket->fd, ( LPSOCKADDR )&addr, sizeof( addr ) ) != 0 )
	{
		logging.error( "Could not open socket on %s:%d", ip.c_str(), port );
		return false;
	}

	listen( socket->fd, 0xFFFF );

	socket->port = port;
	socket->channel = channel;
	socket->type = RealmSocketType::TCP;
	socket->flag.is_listener = true;

	// Set the socket to non-blocking
	u_long non_blocking = 1;
	ioctlsocket( socket->fd, FIONBIO, &non_blocking );

	socket_list.push_back( socket );

	logging.information( "Open TCP Listener on %s:%d", ip.c_str(), port );

	return true;
}

bool CSocketManager::open_udp_listener( std::string ip, uint16_t port, RealmChannelType channel )
{
	struct sockaddr_in addr;
	auto socket = std::make_shared< CRealmSocket >();

	if( INVALID_SOCKET == ( socket->fd = ::socket( PF_INET, SOCK_DGRAM, IPPROTO_UDP ) ) )
	{
		return false;
	}

	memset( &addr, 0, sizeof( addr ) );
	addr.sin_family = AF_INET;
	addr.sin_port = htons( port );
	addr.sin_addr.s_addr = ( "0.0.0.0" != ip )
		? inet_addr( ip.c_str() )
		: htonl( INADDR_ANY );

	if( bind( socket->fd, ( LPSOCKADDR )&addr, sizeof( addr ) ) != 0 )
	{
		logging.error( "Could not open socket on %s:%d", ip.c_str(), port );
		return false;
	}

	socket->port = port;
	socket->channel = channel;
	socket->type = RealmSocketType::UDP;
	socket->flag.is_listener = true;

	// Set the socket to non-blocking
	u_long non_blocking = 1;
	ioctlsocket( socket->fd, FIONBIO, &non_blocking );

	socket_list.push_back( socket );

	logging.information( "Open UDP Listener on %s:%d", ip.c_str(), port );

	return false;
}

bool CSocketManager::get_message( sptr_socket &s, sptr_packet &p )
{
    std::pair< sptr_socket, sptr_packet > pair;
	
    if( !pending_packets.front_and_pop( pair ) )
    {
        return false;
    }
	
	s = pair.first;
	p = std::move( pair.second );

	return true;
}

void CSocketManager::accept_new_tcp( sptr_socket from_socket )
{
	struct sockaddr_in listen_in;
	int listenLen = sizeof( listen_in );
	SOCKET receive_socket = INVALID_SOCKET;
	std::string connection_ip;
	bool is_friendly_connection = false;

	if( ( receive_socket = accept( from_socket->fd, ( struct sockaddr* ) &listen_in, &listenLen ) ) == -1 )
	{
		return;
	}

	connection_ip = inet_ntoa( listen_in.sin_addr );

	auto new_socket = std::make_shared< CRealmSocket >();
	new_socket->fd = receive_socket;
	new_socket->peer_ip_address = inet_ntoa( listen_in.sin_addr );
	new_socket->port = ntohs( listen_in.sin_port );
	new_socket->channel = from_socket->channel;

	if( from_socket->channel == RealmChannelType::GATEWAY )
	{
		logging.information( "[GATEWAY] : New connection from %s", ( *new_socket ).peer_ip_address.c_str() );
	}
	else if( from_socket->channel == RealmChannelType::GAME )
	{
		logging.information( "[GAME] : New connection from %s", ( *new_socket ).peer_ip_address.c_str() );
	}
	
	new_socket->last_recv_time = net_time.GetAppMilliTime();
	new_socket->last_send_time = net_time.GetAppMilliTime();

	accept_list.push_back( new_socket );
}

void CSocketManager::accept_new_udp( sptr_socket s, sockaddr_in addr )
{
	auto new_socket = std::make_shared< CRealmSocket >();
	new_socket->fd = s->fd;
	new_socket->remote_address = addr;
	new_socket->channel = s->channel;
	new_socket->type = RealmSocketType::UDP;

	accept_list.push_back( new_socket );
}

void CSocketManager::notify_socket( sptr_socket s, SocketNotice command )
{
	auto p = std::make_unique< PacketBuffer >( 0xFFFF, 0, 18 );
	p->write_u32( (int)command );
	pending_packets.push( std::make_pair( s, std::move( p ) ) );
}

void CSocketManager::check_socket_problem( sptr_socket sock )
{
	if( sock->flag.disconnected ) return;
	if( sock->flag.is_listener ) return;

	auto now_time = net_time.GetAppMilliTime();

	if( ( now_time - sock->last_recv_time ) > 60000.0 )
	{
		sock->flag.disconnected = 1;
		notify_socket( sock, SocketNotice::DISCONNECTED );
	}
}

void CSocketManager::net_worker()
{
	struct timeval timeout = { 0, 0 };
	FD_SET readfd, writefd;

	while( run_worker )
	{
		net_time.Tick();
		max_fd = 0;
		FD_ZERO( &readfd );
		FD_ZERO( &writefd );

		// Check for any new connections
		for( auto &s : accept_list )
		{
			socket_list.push_back( s );
			notify_socket( s, SocketNotice::NEW_CONNECTION );
		}

		accept_list.clear();

		auto socket = socket_list.begin();

		while( socket != socket_list.end() )
		{
			if( ( *socket )->fd == INVALID_SOCKET || ( *socket )->flag.disconnected )
			{
				socket = socket_list.erase( socket );
				continue;
			}

			FD_SET( ( *socket )->fd, &readfd );
			FD_SET( ( *socket )->fd, &writefd );
			
			max_fd = std::max< SOCKET >( max_fd, ( *socket )->fd + 1 );

			socket++;
		}

		if( select( max_fd, &readfd, &writefd, NULL, &timeout ) <= 0 )
		{
			std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );
			continue;
		}

		for( auto &s : socket_list )
		{
			if( s->type == RealmSocketType::TCP )
			{
				if( FD_ISSET( s->fd, &readfd ) )
				{
					TCP_TryReadData( s );
				}
				else if( FD_ISSET( s->fd, &writefd ) )
				{
					TCP_TryWriteData( s );
				}
			}
			else if( s->type == RealmSocketType::UDP )
			{
				if( FD_ISSET( s->fd, &readfd ) )
				{
					UDP_TryReadData( s );
				}
				else if( FD_ISSET( s->fd, &writefd ) )
				{
					UDP_TryWriteData( s );
				}
			}
		}
	}
}

void CSocketManager::TCP_TryReadData( sptr_socket socket )
{
	// This socket is a listener, so we want to accept a connection
	if( socket->flag.is_listener )
	{
		accept_new_tcp( socket );
		return;
	}

	if( socket->read_buffer.size() >= MAX_READ_SIZE )
	{
		return;
	}

	int32_t read_size = 0;

	if( ( read_size = recv( socket->fd, reinterpret_cast< char* >( &dirty_read_buffer[ 0 ] ), MAX_READ_SIZE, 0 ) ) <= 0 )
	{
		switch( WSAGetLastError() )
		{
			case WSAECONNRESET:
			{
				logging.information( "Connection %s reset by peer.", socket->peer_ip_address.c_str() );
			} break;
		}

		socket->flag.disconnected = 1;
		notify_socket( socket, SocketNotice::DISCONNECTED );
		return;
	}

	// TODO: Parse the data from 'dirty_read_buffer' directly if 'socket->read_buffer' is empty.
	// Otherwise merge them and parse the data.
	std::copy( std::begin( dirty_read_buffer ), std::begin( dirty_read_buffer ) + read_size, std::back_inserter( socket->read_buffer ) );

	// Parse packets out from the incoming data.
	while( !socket->read_buffer.empty() )
	{
		// Read the packet length from the first 4 bytes and flip it
		uint32_t length = _byteswap_ulong( *( uint32_t * )&socket->read_buffer[ 0x00 ] );

		if( length > socket->read_buffer.size() )
		{
			socket->flag.want_more_read_data = 1;
			break;
		}

		if( length == 0x04 )
		{
			// This is a keep alive packet, I think.
			// The client drops them as well.
			socket->read_buffer.erase( socket->read_buffer.begin(), socket->read_buffer.begin() + length );
			continue;
		}

		auto p = std::make_shared< PacketBuffer >( socket->read_buffer, length );

		logging.packet( p->buffer, false );

		socket->read_buffer.erase(
			socket->read_buffer.begin(),
			socket->read_buffer.begin() + length );

		pending_packets.push( std::make_pair( socket, std::move( p ) ) );
	}

	socket->last_recv_time = net_time.GetAppMilliTime();
	socket->latency = abs( socket->last_recv_time - socket->last_send_time );
}

void CSocketManager::TCP_TryWriteData( sptr_socket socket )
{
	if( socket->write_queue.empty() )
	{
		return;
	}

	if( false == socket->write_mutex.try_lock() )
	{
		return;
	}

	size_t write_offset = 0;

	while( !socket->write_queue.empty() )
	{
		auto &packet = socket->write_queue.front();
		auto total_length = packet->write_position - socket->last_write_position;

		auto chunk_size = std::min< int32_t >( total_length, MAX_TCP_SEND_SIZE );
		auto write_size = send( socket->fd, reinterpret_cast< char * >( &packet->buffer[ socket->last_write_position ] ), chunk_size, 0 );

		// Socket Error
		if( write_size == SOCKET_ERROR )
		{
			socket->flag.disconnected = 1;
			break;
		}

		// Not all data sent
		if( write_size < total_length )
		{
			socket->flag.want_more_write_data = 1;
			socket->last_write_position += write_size;
			break;
		}

		socket->write_queue.pop_front();

		socket->flag.want_more_write_data = 0;
		socket->last_write_position = 0;

	}

	socket->write_mutex.unlock();

	socket->last_send_time = net_time.GetAppMilliTime();
	socket->latency = abs( socket->last_recv_time - socket->last_send_time );
}

void CSocketManager::UDP_TryReadData( sptr_socket socket )
{
	if( socket->read_buffer.size() >= MAX_READ_SIZE )
	{
		return;
	}

	int32_t read_size = 0;
	SOCKADDR_IN addr;
	int addrLen = sizeof( addr );

	read_size = recvfrom( socket->fd, reinterpret_cast< char * >( &dirty_read_buffer[ 0 ] ), MAX_READ_SIZE, 0, ( SOCKADDR * )&addr, &addrLen );

	if( read_size <= 0 )
	{
		return;
	}

	if( socket->flag.is_listener )
	{
		accept_new_udp( socket, addr );
	}

	logging.packet( dirty_read_buffer, read_size, false );

	auto packet = std::make_shared< PacketBuffer >( dirty_read_buffer, read_size );

	logging.packet( packet->buffer, false );

	pending_packets.push( std::make_pair( socket, std::move( packet ) ) );
}

void CSocketManager::UDP_TryWriteData( sptr_socket s )
{
	if( s->write_queue.empty() )
	{
		return;
	}

	if( false == s->write_mutex.try_lock() )
	{
		return;
	}


	auto &packet = s->write_queue.front();
	auto total_length = packet->write_position;

	auto chunk_size = std::min< int32_t >( total_length, MAX_UDP_SEND_SIZE );
	auto write_size = sendto( s->fd, reinterpret_cast< char * >( &packet->buffer[ 0 ] ), chunk_size, 0, ( SOCKADDR * )&s->remote_address, sizeof( s->remote_address ) );

	// Socket Error
	if( write_size == SOCKET_ERROR )
	{
		s->flag.disconnected = 1;
	}

	s->write_queue.pop_front();

	s->write_mutex.unlock();
}
