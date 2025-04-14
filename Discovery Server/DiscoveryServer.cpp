// ╔╗╔╔═╗╦═╗╦═╗╔═╗╔╦╗╦ ╦                          
// ║║║║ ║╠╦╝╠╦╝╠═╣ ║ ╠═╣                          
// ╝╚╝╚═╝╩╚═╩╚═╩ ╩ ╩ ╩ ╩                          
// ╔╦╗╦╔═╗╔═╗╔═╗╦  ╦╔═╗╦═╗╦ ╦  ╔═╗╔═╗╦═╗╦  ╦╔═╗╦═╗
//  ║║║╚═╗║  ║ ║╚╗╔╝║╣ ╠╦╝╚╦╝  ╚═╗║╣ ╠╦╝╚╗╔╝║╣ ╠╦╝
// ═╩╝╩╚═╝╚═╝╚═╝ ╚╝ ╚═╝╩╚═ ╩   ╚═╝╚═╝╩╚═ ╚╝ ╚═╝╩╚═

#include "../global_define.h"

DiscoveryServer::DiscoveryServer()
{
	m_running = false;

	m_socket = INVALID_SOCKET;
	m_recvBuffer.resize( 1024 );
}

DiscoveryServer::~DiscoveryServer()
{
	Log::Info( "Discovery Server stopped." );
}

void DiscoveryServer::Start( std::string ip, int32_t port )
{
	m_socket = socket( AF_INET, SOCK_DGRAM, 0 );

	if( m_socket == INVALID_SOCKET )
	{
		Log::Error( "Failed to create socket." );
		return;
	}

	sockaddr_in service;
	service.sin_family = AF_INET;
	service.sin_port = htons( port );

	if( ip == "0.0.0.0" )
	{
		service.sin_addr.s_addr = ADDR_ANY;
	}
	else
	{
		service.sin_addr.s_addr = inet_addr( ip.c_str() );
	}

	if( bind( m_socket, ( SOCKADDR * )&service, sizeof( service ) ) == SOCKET_ERROR )
	{
		Log::Error( "Failed to bind socket." );
		closesocket( m_socket );
		return;
	}

	m_running = true;
	m_thread = std::thread( &DiscoveryServer::Run, this );

	Log::Info( "Discovery Server started %s:%d", ip.c_str(), port );
}

void DiscoveryServer::Stop()
{
	m_running = false;
	if( m_thread.joinable() )
	{
		m_thread.join();
	}
}

void DiscoveryServer::Run()
{
	sockaddr_in clientAddr;
	int clientAddrLen = sizeof( clientAddr );

	while( m_running )
	{
		std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );

		auto bytesReceived = recvfrom( m_socket, ( char * )m_recvBuffer.data(), 1024, 0, ( struct sockaddr * )&clientAddr, &clientAddrLen );

		if( bytesReceived == SOCKET_ERROR || bytesReceived < 4 )
		{
			continue;
		}

		ProcessPacket( &clientAddr, std::make_shared< ByteStream >( m_recvBuffer.data(), bytesReceived ) );
	}
}

void DiscoveryServer::ProcessPacket( sockaddr_in *clientAddr, sptr_byte_stream stream )
{
	if( 0x20 != stream->read_u32() )
	{
		return;
	}

	auto encryptedBytes = stream->read_bytes( 0x20 );
	auto decryptedBytes = RealmCrypt::decryptSymmetric( encryptedBytes );

	std::wstring sessionId( 16, L'\0' );
	std::memcpy( sessionId.data(), decryptedBytes.data(), 0x20 );

	if( sessionId.empty() || sessionId.size() != 16 )
	{
		Log::Error( "Invalid session id." );
		return;
	}

	// Get the users remote IP and Port for discovery.
	auto remoteIp = inet_ntoa( clientAddr->sin_addr );
	auto remotePort = ntohs( clientAddr->sin_port );

	Log::Debug( "Discovery Handshake from %s:%d", remoteIp, remotePort );

	// Find the user associated with the session ID
	auto user = RealmUserManager::Get().GetUser( sessionId );
	if( user == nullptr )
	{
		Log::Error( "User not found! [%S]", sessionId.c_str() );
		return;
	}

	if( remoteIp != user->sock->remote_ip )
	{
		Log::Error( "Discovery Handshake from invalid IP!" );
		return;
	}

	// Initialize our discovery information
	user->m_discoveryAddr = remoteIp;
	user->m_discoveryPort = remotePort;

	if( user->m_isHost )
	{
		GameSessionManager::Get().RequestOpen( user );
	}
	else
	{
		GameSessionManager::Get().RequestJoin( user );
	}
}