#pragma once

#include <memory>
#include <array>
#include <string>

struct sockaddr_in_hash {
	std::size_t operator()( const sockaddr_in &addr ) const
	{
		std::size_t h1 = std::hash<int>()( addr.sin_family );
		std::size_t h2 = std::hash<unsigned short>()( addr.sin_port );
		std::size_t h3 = std::hash<unsigned long>()( addr.sin_addr.s_addr );
		return h1 ^ ( h2 << 1 ) ^ ( h3 << 2 );
	}
};

struct sockaddr_in_equal {
	bool operator()( const sockaddr_in &lhs, const sockaddr_in &rhs ) const
	{
		return lhs.sin_family == rhs.sin_family &&
			lhs.sin_port == rhs.sin_port &&
			lhs.sin_addr.s_addr == rhs.sin_addr.s_addr;
	}
};

class DiscoverySession {
public:
	static std::shared_ptr< DiscoverySession > Create( sptr_udp_socket socket );

	DiscoverySession( sptr_udp_socket socket );
	~DiscoverySession();

	sptr_udp_socket m_socket;
	std::wstring m_ownerSessionId;

	std::array< std::wstring, 4 > m_userSessionIds;
};

typedef std::shared_ptr< DiscoverySession > sptr_discovery_record;