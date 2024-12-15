#pragma once

#include <mutex>
#include <vector>
#include <memory>

#include "packet.h"

enum class RealmSocketType
{
	TCP = 0,
	UDP,
};

enum class RealmChannelType
{
	INVALID = 0,
	GATEWAY,
	GAME,
	DISCOVERY,
};

class CRealmSocket
{
public:
	CRealmSocket();
	~CRealmSocket();

	void send( const sptr_packet p );

	struct s_flag
	{
		bool disconnected;
		bool is_listener;
		bool want_more_read_data;
		bool want_more_write_data;
	} flag;

	RealmSocketType type;
	RealmChannelType channel;

	SOCKET fd;
	uint16_t port;
	sockaddr_in local_address;
	sockaddr_in remote_address;
	std::string peer_ip_address;

	uint32_t last_write_position;

	double_t latency;
	double_t last_recv_time;
	double_t last_send_time;

	std::mutex write_mutex;
	std::mutex read_mutex;

	std::vector< uint8_t >		read_buffer;
	std::list< sptr_packet >	read_queue;
	std::list< sptr_packet >	write_queue;
};

typedef std::shared_ptr< CRealmSocket > sptr_socket;