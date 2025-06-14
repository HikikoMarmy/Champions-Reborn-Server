#pragma once

#include <memory>
#include <string>
#include <thread>
#include <mutex>
#include <atomic>
#include <vector>
#include <winsock2.h>
#include <ws2tcpip.h>

#include "../Common/ByteStream.h"

class DiscoveryServer {
public:
	static DiscoveryServer &Get()
	{
		static DiscoveryServer instance;
		return instance;
	}

	DiscoveryServer( const DiscoveryServer & ) = delete;
	DiscoveryServer &operator=( const DiscoveryServer & ) = delete;
	DiscoveryServer();
	~DiscoveryServer();

	void Start( std::string ip, int32_t port );
	void Stop();
	void Run();
	bool isRunning() const
	{
		return m_running;
	}

private:
	void ProcessPacket( sockaddr_in *clientAddr, sptr_byte_stream stream );

	std::atomic< bool > m_running;
	std::thread m_thread;

	SOCKET m_socket;
	std::vector< unsigned char >	m_recvBuffer;
};