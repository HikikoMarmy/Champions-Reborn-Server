#pragma once

#include <vector>
#include <set>
#include <thread>
#include <string>
#include <memory>
#include "packet.h"
#include "socket.h"
#include "../misc/Timer.h"
#include "../misc/threadsafe_queue.hpp"

enum class SocketNotice
{
	NEW_CONNECTION = 1,
	DISCONNECTED = 2,
};

class CSocketManager
{
private:
	const uint32_t MAX_TCP_SEND_SIZE = 65535;
	const uint32_t MAX_UDP_SEND_SIZE = 1024;
	const uint32_t MAX_READ_SIZE = 65535;
	const uint32_t MAX_SOCKETS = 1024;

	CTimer	net_time;
	std::vector< uint8_t > dirty_read_buffer;
	std::vector< sptr_socket > accept_list;
	std::vector< sptr_socket > socket_list;
	threadsafe_queue< std::pair< sptr_socket, sptr_packet > > pending_packets;

	bool run_worker, wait_end;
	SOCKET max_fd;
public:
	CSocketManager();
	~CSocketManager();

	bool initialize();
	bool open_tcp_listener( std::string ip, uint16_t port, RealmChannelType channel );
	bool open_udp_listener( std::string ip, uint16_t port, RealmChannelType channel );
	bool get_message( sptr_socket &s, sptr_packet &p );
	
private:
	void net_worker();
	void accept_new_tcp( sptr_socket s );
	void accept_new_udp( sptr_socket s, sockaddr_in addr );
	void notify_socket( sptr_socket s, SocketNotice notice );
	void check_socket_problem( sptr_socket s );

	void TCP_TryReadData( sptr_socket s );
	void TCP_TryWriteData( sptr_socket s );

	void UDP_TryReadData( sptr_socket s );
	void UDP_TryWriteData( sptr_socket s );
};