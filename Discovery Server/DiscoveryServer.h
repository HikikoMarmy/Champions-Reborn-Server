#pragma once

class DiscoveryServer
{
private:
	static inline std::unique_ptr< DiscoveryServer > m_instance;
	static inline std::mutex m_mutex;

public:
	static DiscoveryServer& Get()
	{
		std::lock_guard< std::mutex > lock( m_mutex );
		if( m_instance == nullptr )
		{
			m_instance.reset( new DiscoveryServer() );
		}

		return *m_instance;
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
	void ProcessPacket(sockaddr_in* clientAddr, sptr_byte_stream stream);

	std::atomic< bool > m_running;
	std::thread m_thread;

	SOCKET m_socket;
	std::vector< unsigned char >	m_recvBuffer;
};