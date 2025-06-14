// ╔╗╔╔═╗╦═╗╦═╗╔═╗╔╦╗╦ ╦               
// ║║║║ ║╠╦╝╠╦╝╠═╣ ║ ╠═╣               
// ╝╚╝╚═╝╩╚═╩╚═╩ ╩ ╩ ╩ ╩               
// ╦  ╔═╗╔╗ ╔╗ ╦ ╦  ╔═╗╔═╗╦═╗╦  ╦╔═╗╦═╗
// ║  ║ ║╠╩╗╠╩╗╚╦╝  ╚═╗║╣ ╠╦╝╚╗╔╝║╣ ╠╦╝
// ╩═╝╚═╝╚═╝╚═╝ ╩   ╚═╝╚═╝╩╚═ ╚╝ ╚═╝╩╚═

#include "LobbyServer.h"

#include "../Game/RealmUserManager.h"
#include "../Network/Events.h"
#include "../../logging.h"

LobbyServer::LobbyServer()
{
	m_running = false;
	m_conSocket = INVALID_SOCKET;
	m_rtaSocket = INVALID_SOCKET;

	for( int i = 0; i < 8; i++ )
	{
		m_conGatewaySocket[ i ] = INVALID_SOCKET;
		m_rtaGatewaySocket[ i ] = INVALID_SOCKET;
	}

	m_clientSockets.clear();
	m_recvBuffer.resize( 1024 );
}

LobbyServer::~LobbyServer()
{
	Log::Info( "Lobby Server stopped." );

	for( auto &socket : m_clientSockets )
	{
		if( socket->fd != INVALID_SOCKET )
		{
			closesocket( socket->fd );
		}
	}

	for( auto &sock : m_conGatewaySocket )
	{
		if( sock != INVALID_SOCKET )
		{
			closesocket( sock );
		}
	}

	for( auto &sock : m_rtaGatewaySocket )
	{
		if( sock != INVALID_SOCKET )
		{
			closesocket( sock );
		}
	}
}

void LobbyServer::Start( std::string ip )
{
	for( int i = 0; i < 8; i++ )
	{
		m_conGatewaySocket[ i ] = OpenNetworkSocket( ip, 40800 + i );
		if( m_conGatewaySocket[ i ] == INVALID_SOCKET )
			return;

		m_rtaGatewaySocket[ i ] = OpenNetworkSocket( ip, 40810 + i );
		if( m_rtaGatewaySocket[ i ] == INVALID_SOCKET )
			return;
	}

	m_conSocket = OpenNetworkSocket( ip, 40900 );
	m_rtaSocket = OpenNetworkSocket( ip, 40910 );

	m_running = true;
	m_thread = std::thread( &LobbyServer::Run, this );

	Log::Info( "Lobby Server started" );
}

void LobbyServer::Stop()
{
	m_running = false;
	if( m_thread.joinable() )
	{
		m_thread.join();
	}
}

SOCKET LobbyServer::OpenNetworkSocket( std::string ip, int32_t port )
{
	SOCKET sock = ::WSASocket( AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED );
	if( sock == INVALID_SOCKET )
	{
		Log::Error( "WSASocket() failed on port %u", port );
		return INVALID_SOCKET;
	}

	sockaddr_in service{};
	service.sin_family = AF_INET;
	service.sin_port = htons( port );

	if( ip == "0.0.0.0" )
	{
		service.sin_addr.s_addr = INADDR_ANY;
	}
	else
	{
		if( InetPtonA( AF_INET, ip.c_str(), &service.sin_addr ) != 1 )
		{
			Log::Error( "Invalid IP address format: %s", ip.c_str() );
			closesocket( sock );
			return INVALID_SOCKET;
		}
	}

	if( bind( sock, reinterpret_cast< SOCKADDR * >( &service ), sizeof( service ) ) == SOCKET_ERROR )
	{
		Log::Error( "bind() failed on port %u", port );
		closesocket( sock );
		return INVALID_SOCKET;
	}

	if( listen( sock, SOMAXCONN ) == SOCKET_ERROR )
	{
		Log::Error( "listen() failed on port %u", port );
		closesocket( sock );
		return INVALID_SOCKET;
	}

	Log::Info( "Socket Opened on %s:%d", ip.c_str(), port );

	return sock;
}

void LobbyServer::Run()
{
	FD_SET readSet;
	FD_SET writeSet;

	timeval timeout = { 0, 1000 };

	while( m_running )
	{
		FD_ZERO( &readSet );
		FD_ZERO( &writeSet );

		FD_SET( m_conSocket, &readSet );
		FD_SET( m_rtaSocket, &readSet );

		for( auto &sock : m_conGatewaySocket )
		{
			if( sock != INVALID_SOCKET )
			{
				FD_SET( sock, &readSet );
			}
		}

		for( auto &sock : m_rtaGatewaySocket )
		{
			if( sock != INVALID_SOCKET )
			{
				FD_SET( sock, &readSet );
			}
		}

		CheckSocketProblem();

		for( const auto &client : m_clientSockets )
		{
			FD_SET( client->fd, &readSet );
			FD_SET( client->fd, &writeSet );
		}

		auto result = select( 0, &readSet, &writeSet, NULL, &timeout );

		if( result == SOCKET_ERROR )
		{
			std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );
			continue;
		}

		for( auto &sock : m_conGatewaySocket )
		{
			if( sock != INVALID_SOCKET && FD_ISSET( sock, &readSet ) )
			{
				AcceptGateway( sock, RealmGameType::CHAMPIONS_OF_NORRATH );
			}
		}

		for( auto &sock : m_rtaGatewaySocket )
		{
			if( sock != INVALID_SOCKET && FD_ISSET( sock, &readSet ) )
			{
				AcceptGateway( sock, RealmGameType::RETURN_TO_ARMS );
			}
		}

		if( FD_ISSET( m_conSocket, &readSet ) )
		{
			AcceptClient( m_conSocket, RealmGameType::CHAMPIONS_OF_NORRATH );
		}

		if( FD_ISSET( m_rtaSocket, &readSet ) )
		{
			AcceptClient( m_rtaSocket, RealmGameType::RETURN_TO_ARMS );
		}

		for( auto &client : m_clientSockets )
		{
			if( FD_ISSET( client->fd, &readSet ) )
			{
				ReadSocket( client );
			}

			if( FD_ISSET( client->fd, &writeSet ) )
			{
				WriteSocket( client );
			}
		}
	}
}

void LobbyServer::CheckSocketProblem()
{
	auto now = std::chrono::steady_clock::now();

	for( auto it = m_clientSockets.begin(); it != m_clientSockets.end(); )
	{
		auto &socket = *it;
		auto elapsed = std::chrono::duration_cast< std::chrono::seconds >( now - socket->last_recv_time );

		// Check for client timeouts
		if( elapsed.count() > 30 )
		{
			socket->flag.disconnected_forced = true;
			Log::Info( "[LOBBY] Client Timeout : (%s)", socket->remote_ip.c_str() );
		}

		// Check if we're waiting to disconnect after sending buffered data
		if( socket->flag.disconnected_wait && socket->m_pendingWriteBuffer.empty() )
		{
			socket->flag.disconnected_forced = true;
		}

		if( socket->flag.disconnected_forced )
		{
			RealmUserManager::Get().RemoveUser( socket );
			Log::Info( "[LOBBY] Client Disconnected : (%s)", socket->remote_ip.c_str() );
			it = m_clientSockets.erase( it );
		}
		else
		{
			++it;
		}
	}
}

void LobbyServer::AcceptGateway( SOCKET socket, RealmGameType clientType )
{
	sockaddr_in clientInfo{};
	int32_t addrSize = sizeof( clientInfo );

	SOCKET clientSocket = accept( socket, ( SOCKADDR * )&clientInfo, &addrSize );
	if( clientSocket == INVALID_SOCKET )
	{
		Log::Error( "accept() failed" );
		return;
	}

	auto new_socket = std::make_shared< RealmSocket >();
	new_socket->fd = clientSocket;
	new_socket->remote_addr = clientInfo;
	new_socket->remote_ip = inet_ntoa( clientInfo.sin_addr );
	new_socket->remote_port = ntohs( clientInfo.sin_port );
	m_clientSockets.push_back( new_socket );

	Log::Info( "New Gateway Client Connected : (%s)", new_socket->remote_ip.c_str() );
}

void LobbyServer::AcceptClient( SOCKET socket, RealmGameType clientType )
{
	sockaddr_in clientInfo{};
	int32_t addrSize = sizeof( clientInfo );

	SOCKET clientSocket = accept( socket, ( SOCKADDR * )&clientInfo, &addrSize );
	if( clientSocket == INVALID_SOCKET )
	{
		Log::Error( "accept() failed" );
		return;
	}

	auto new_socket = std::make_shared< RealmSocket >();
	new_socket->fd = clientSocket;
	new_socket->remote_addr = clientInfo;
	new_socket->remote_ip = inet_ntoa( clientInfo.sin_addr );
	new_socket->remote_port = ntohs( clientInfo.sin_port );

	m_clientSockets.push_back( new_socket );

	RealmUserManager::Get().CreateUser( new_socket, clientType );

	Log::Info( "New Client Connected : (%s)", new_socket->remote_ip.c_str() );
}

void LobbyServer::ReadSocket( sptr_socket socket )
{
	if( socket->flag.disconnected_wait )
	{
		return;
	}

	const auto bytesReceived = recv( socket->fd, reinterpret_cast< char * >( m_recvBuffer.data() ), static_cast< int >( m_recvBuffer.size() ), 0 );

	if( bytesReceived == SOCKET_ERROR )
	{
		Log::Info( "Socket Error [%d].", WSAGetLastError() );
		socket->flag.disconnected_wait = true;
		return;
	}

	if( bytesReceived == 0 )
	{
		socket->flag.disconnected_wait = true;
		return;
	}

	socket->last_recv_time = std::chrono::steady_clock::now();

	// Append received data to socket's pending buffer
	socket->m_pendingReadBuffer.insert(
		socket->m_pendingReadBuffer.end(),
		m_recvBuffer.begin(),
		m_recvBuffer.begin() + bytesReceived
	);

	// Process packets in the buffer
	while( socket->m_pendingReadBuffer.size() >= 4 )
	{
		int32_t packetSize = ntohl( *reinterpret_cast< const int32_t * >( &socket->m_pendingReadBuffer[ 0 ] ) );

		if( packetSize <= 0 || packetSize > 2048 )
		{
			Log::Error( "Invalid packet size: %d. Disconnecting client.", packetSize );
			socket->flag.disconnected_wait = true;
			break;
		}

		if( socket->m_pendingReadBuffer.size() < static_cast< size_t >( packetSize ) )
		{
			break;
		}

		auto stream = std::make_shared< ByteBuffer >( socket->m_pendingReadBuffer.data() + 4, packetSize - 4 );

		// Erase the packet from the buffer
		socket->m_pendingReadBuffer.erase(
			socket->m_pendingReadBuffer.begin(),
			socket->m_pendingReadBuffer.begin() + packetSize
		);

		if( stream->m_buffer.size() == 0 ) break;

		// Process the packet
		HandleRequest( socket, stream );
	}
}

void LobbyServer::WriteSocket( sptr_socket socket )
{
	if( socket->flag.disconnected_wait || socket->m_pendingWriteBuffer.empty() )
		return;

	socket->last_send_time = std::chrono::steady_clock::now();

	size_t totalBytesSent = 0;
	const size_t bufferSize = socket->m_pendingWriteBuffer.size();

	while( totalBytesSent < bufferSize )
	{
		const size_t remaining = bufferSize - totalBytesSent;
		const int chunkSize = static_cast< int >( std::min<size_t>( remaining, 1024 ) );

		int bytesSent = send(
			socket->fd,
			reinterpret_cast< const char * >( socket->m_pendingWriteBuffer.data() + totalBytesSent ),
			chunkSize,
			0
		);

		if( bytesSent == SOCKET_ERROR )
		{
			const int err = WSAGetLastError();
			if( err == WSAEWOULDBLOCK )
				break;

			Log::Error( "Send failed: %d", err );
			socket->flag.disconnected_wait = true;
			return;
		}

		if( bytesSent == 0 )
		{
			socket->flag.disconnected_wait = true;
			return;
		}

		totalBytesSent += bytesSent;

		if( bytesSent < chunkSize )
		{
			break;
		}
	}

	std::lock_guard< std::mutex > lock( socket->write_mutex );

	socket->m_pendingWriteBuffer.erase(
		socket->m_pendingWriteBuffer.begin(),
		socket->m_pendingWriteBuffer.begin() + totalBytesSent
	);
}

void LobbyServer::HandleRequest( sptr_socket socket, sptr_byte_stream stream )
{
	auto packetId = stream->read< uint16_t >();
	stream->set_position( 0 );

	Log::Debug( "Event Request %04X", packetId );

	//Log::Packet( stream->m_buffer, stream->m_buffer.size(), false );

	auto it = REQUEST_EVENT.find( packetId );
	if( it == REQUEST_EVENT.end() )
	{
		Log::Error( "[LOBBY] Unknown packet id : 0x%04X", packetId );
		Log::Packet( stream->m_buffer, stream->m_buffer.size(), false );
		return;
	}

	auto request = it->second();
	//auto user = RealmUserManager::Get().FindUserBySocket( socket );
	//
	//if( user == nullptr )
	//{
	//	Log::Error( "User not found!" );
	//	socket->flag.disconnected_wait = true;
	//	return;
	//}

	if( auto res = request->ProcessRequest( socket, stream ) )
	{
		socket->send( res );
	}
}