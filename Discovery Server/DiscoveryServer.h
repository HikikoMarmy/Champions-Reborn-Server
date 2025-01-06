#pragma once

#include <memory>
#include <array>
#include <unordered_map>

#include "DiscoverySession.h"

struct UserDiscoveryInfo {
	std::string m_ip;
	int32_t m_port;
	uint64_t m_lastUpdateTime;
};

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

	void UpdateDiscoveryInfo( std::wstring sessionId, std::string ip, int32_t port );
	std::optional<UserDiscoveryInfo> GetDiscoveryInfo( const std::wstring &sessionId ) const;

private:
	void ReadSocket();
	void HandleDiscoveryUpdate( sockaddr_in *clientAddr, sptr_byte_stream stream );

	static inline std::unique_ptr< DiscoveryServer > m_instance;
	static inline std::mutex m_mutex;

	std::atomic< bool > m_running;
	std::thread m_thread;

	SOCKET m_socket;
	std::vector< uint8_t > m_recvBuffer;

	std::unordered_map< std::wstring, UserDiscoveryInfo > m_userDiscoveryInfo;
};