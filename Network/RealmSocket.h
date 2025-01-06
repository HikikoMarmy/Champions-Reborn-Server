#pragma once

#include <vector>
#include <queue>
#include <memory>
#include <mutex>

class GenericResponse;
typedef std::shared_ptr< GenericResponse > sptr_generic_response;

class GenericMessage;

class RealmSocket
{
public:
	RealmSocket();
	~RealmSocket();

	virtual void send( const sptr_byte_stream stream ) = 0;
	virtual void send( const ByteStream &stream ) = 0;

	struct s_flag
	{
		bool disconnected;
		bool is_listener;
		bool want_more_read_data;
		bool want_more_write_data;
	} flag;

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
	uint16_t port;
	sockaddr_in local_address;
	sockaddr_in remote_address;

	std::string peer_ip_address;
	int32_t peer_port;

	uint32_t last_write_position;

	uint64_t latency;
	uint64_t last_recv_time;
	uint64_t last_send_time;

	std::mutex write_mutex;
	std::mutex read_mutex;

	std::vector< uint8_t >		read_buffer;
};

class RealmTCPSocket : public RealmSocket
{
private:
	const size_t WRITE_BUFFER_SIZE = 65535;

public:
	RealmTCPSocket();
	~RealmTCPSocket();

	// Comparison operator for sorting
	bool operator<( const RealmTCPSocket &rhs ) const
	{
		return fd < rhs.fd;
	}

	// Comparison operator for comparing
	bool operator==( const RealmTCPSocket &rhs ) const
	{
		return fd == rhs.fd;
	}

	void send( const sptr_byte_stream stream ) override;
	void send( const ByteStream &stream ) override;
	void send( const sptr_generic_response response );
	void send( GenericMessage &message );

public:
	std::vector< uint8_t > m_pendingWriteBuffer;
	std::vector< uint8_t > m_pendingReadBuffer;
};

typedef std::shared_ptr< RealmTCPSocket > sptr_tcp_socket;

class RealmUDPSocket : public RealmSocket
{
private:
	const int DATAGRAM_SIZE = 1024;

public:
	RealmUDPSocket();
	~RealmUDPSocket();

	// Comparison operator for sorting
	bool operator<( const RealmUDPSocket &rhs ) const
	{
		return fd < rhs.fd;
	}

	// Comparison operator for comparing
	bool operator==( const RealmUDPSocket &rhs ) const
	{
		return fd == rhs.fd;
	}

	void send( const sptr_byte_stream stream ) override;
	void send( const ByteStream &stream ) override;

public:
	std::queue< sptr_byte_stream > m_pendingWriteQueue;
};

typedef std::shared_ptr< RealmUDPSocket > sptr_udp_socket;
