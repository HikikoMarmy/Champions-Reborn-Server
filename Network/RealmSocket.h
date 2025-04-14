#pragma once

#include <vector>
#include <mutex>

// Forward Declare GenericResponse
class GenericResponse;
typedef std::shared_ptr< GenericResponse > sptr_generic_response;

// Forward Declare Generic Message
class GenericMessage;

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
		bool disconnected;
		bool is_listener;
		bool want_more_read_data;
		bool want_more_write_data;
	} flag;

	sockaddr_in local_addr;
	sockaddr_in remote_addr;

	std::string remote_ip;
	int32_t remote_port;

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

typedef std::shared_ptr< RealmSocket > sptr_socket;
