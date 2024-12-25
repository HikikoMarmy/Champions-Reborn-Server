#pragma once

#include <memory>
#include <array>
#include <unordered_map>

#include "DiscoverySession.h"

class DiscoveryServer
{
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
	bool OpenDiscoverySocket( std::string ip, int32_t port );
	sptr_udp_socket CreateDiscoverySocket( sockaddr_in *clientAddr );

	sptr_discovery_record CreateNewDiscoveryRecord( sockaddr_in *clientAddr, std::wstring sessionId );
	sptr_discovery_record GetDiscoveryRecord( sockaddr_in *clientAddr );
	std::wstring GetSessionId( sptr_byte_stream stream );

	void AcceptNewClient( sockaddr_in *clientAddr, sptr_byte_stream stream );
	void ReadSocket( sptr_udp_socket socket );
	void WriteSocket( sptr_udp_socket socket );
	void HandleRequest( sockaddr_in *clientAddr, sptr_byte_stream stream );

	void SendDiscoveryClientHandshake( sptr_discovery_record record, sptr_byte_stream stream );
	void SendDiscoveryClientPing( sptr_discovery_record record, sptr_byte_stream stream );

private:
	static inline std::unique_ptr< DiscoveryServer > m_instance;
	static inline std::mutex m_mutex;
	Timer m_timer;

	std::atomic< bool > m_running;
	std::thread m_thread;

	sptr_udp_socket m_socket;
	std::vector< uint8_t > m_recvBuffer;
	std::unordered_map< sockaddr_in, sptr_discovery_record, sockaddr_in_hash, sockaddr_in_equal> m_sessionMap;
};