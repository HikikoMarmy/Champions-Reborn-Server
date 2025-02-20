#pragma once

#include <memory>
#include <array>
#include <unordered_map>

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

	void ProcessDiscoveryInitial( sockaddr_in *clientAddr, sptr_byte_stream stream );

	void ProcessHost(sptr_user user, sptr_byte_stream stream);

	// Joinee Handlers
	void ProcessJoin(sptr_user user, sptr_byte_stream stream);
	void PrepareJoinGame( sptr_user user );
	void PrepareHostGame( sptr_user user );

	void Send_07( sptr_user user );	// Realm Initialize
	void Send_08( sptr_user user );	// Member Info Update
	void Send_09( sptr_user user ); // Member Address Info / Start Game

	bool Process_0C( sptr_user user, sptr_byte_stream stream );	// Member Character Information
	void Send_0C(sptr_user user);	// Joining Member Character Info
	void Send_0D(sptr_user user);

	void Send_Tmp(sockaddr_in* clientAddr);

	std::atomic< bool > m_running;
	std::thread m_thread;

	SOCKET m_socket;
	std::vector< unsigned char >	m_recvBuffer;
};