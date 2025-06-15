#pragma once

#include <memory>
#include <vector>
#include <mutex>
#include <winsock2.h>

#include "GenericNetRequest.h"
#include "GenericNetResponse.h"
#include "GenericNetMessage.h"
#include "../Common/Constant.h"

class RealmSocket
{
private:
	const size_t WRITE_BUFFER_SIZE = 65535;

public:
	RealmSocket();
	~RealmSocket();

	void send( const sptr_generic_response response );
	void send( GenericMessage &message );

	// Comparison operator for sorting
	bool operator<( const RealmSocket &rhs ) const
	{
		return fd < rhs.fd;
	}

	// Comparison operator for comparing
	bool operator==( const RealmSocket &rhs ) const
	{
		return fd == rhs.fd;
	}

	SOCKET fd;

	struct s_flag {
		bool disconnected_wait;
		bool disconnected_forced;
		bool is_listener;
		bool want_more_read_data;
		bool want_more_write_data;
	} flag;

	sockaddr_in local_addr;
	sockaddr_in remote_addr;

	RealmGameType gameType;
	std::string remote_ip;
	uint32_t remote_port;
	uint32_t last_write_position;

	uint64_t latency;
	std::chrono::steady_clock::time_point last_recv_time;
	std::chrono::steady_clock::time_point last_send_time;

	std::mutex write_mutex;
	std::mutex read_mutex;

	std::vector< uint8_t >		read_buffer;

	std::vector< uint8_t > m_pendingWriteBuffer;
	std::vector< uint8_t > m_pendingReadBuffer;
};

using sptr_socket = std::shared_ptr< RealmSocket >;
