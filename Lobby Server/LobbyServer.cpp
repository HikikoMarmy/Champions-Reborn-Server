// ╔═╗ ╦ ╦ ╔═╗ ╔╦╗ ╔═╗ ╦ ╔═╗ ╔╗╔ ╔═╗
// ║   ╠═╣ ╠═╣ ║║║ ╠═╝ ║ ║ ║ ║║║ ╚═╗
// ╚═╝ ╩ ╩ ╩ ╩ ╩ ╩ ╩   ╩ ╚═╝ ╝╚╝ ╚═╝
// ╦  ╔═╗╔╗ ╔╗ ╦ ╦  ╔═╗╔═╗╦═╗╦  ╦╔═╗╦═╗
// ║  ║ ║╠╩╗╠╩╗╚╦╝  ╚═╗║╣ ╠╦╝╚╗╔╝║╣ ╠╦╝
// ╩═╝╚═╝╚═╝╚═╝ ╩   ╚═╝╚═╝╩╚═ ╚╝ ╚═╝╩╚═

#include "LobbyServer.h"

#include "../Game/RealmUserManager.h"
#include "../Network/Events.h"
#include "../../configuration.h"
#include "../../logging.h"

LobbyServer::LobbyServer()
{
	m_running = false;
	m_conSocket.reset();
	m_rtaSocket.reset();

	m_gatewaySockets.clear();

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

	for( auto &socket : m_gatewaySockets )
	{
		if( socket->fd != INVALID_SOCKET )
		{
			closesocket( socket->fd );
		}
	}
}

void LobbyServer::Start( std::string ip )
{
	// Champions of Norrath Gateway Sockets
	for( int i = 0; i < 8; i++ )
	{
		auto gatewaySocket = OpenListenerSocket( ip, 40800 + i, RealmGameType::CHAMPIONS_OF_NORRATH );
		if( nullptr == gatewaySocket ) return;

		gatewaySocket->flag.is_gateway = true;
		m_gatewaySockets.push_back( gatewaySocket );
	}

	// Return to Arms Gateway Sockets
	for( int i = 0; i < 8; i++ )
	{
		auto gatewaySocket = OpenListenerSocket( ip, 40810 + i, RealmGameType::RETURN_TO_ARMS );
		if( nullptr == gatewaySocket ) return;

		gatewaySocket->flag.is_gateway = true;
		m_gatewaySockets.push_back( gatewaySocket );
	}

	m_conSocket = OpenListenerSocket( ip, Config::con_lobby_port, RealmGameType::CHAMPIONS_OF_NORRATH );
	m_rtaSocket = OpenListenerSocket( ip, Config::rta_lobby_port, RealmGameType::RETURN_TO_ARMS );

	m_running = true;
	m_thread = std::thread( &LobbyServer::Run, this );

	Log::Info( "Lobby Server started" );
}

void LobbyServer::Stop()
{
	Log::Info( "Stopping Lobby Server..." );

	for( auto &client : m_clientSockets )
	{
		client->send( NotifyForcedLogout() );
	}

	m_running = false;
	if( m_thread.joinable() )
	{
		m_thread.join();
	}
}

sptr_socket LobbyServer::OpenListenerSocket( std::string ip, int32_t port, RealmGameType type )
{
	SOCKET sock = ::WSASocket( AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED );
	if( sock == INVALID_SOCKET )
	{
		Log::Error( "WSASocket() failed on port {}", port );
		return nullptr;
	}

	sockaddr_in service{};
	service.sin_family = AF_INET;
	service.sin_port = htons( port );

	if( ip == "0.0.0.0" )
	{
		service.sin_addr.s_addr = INADDR_ANY;
	}
	else if( InetPtonA( AF_INET, ip.c_str(), &service.sin_addr) != 1 )
	{
		Log::Error( "Invalid IP address format: {}", ip );
		closesocket( sock );
		return nullptr;
	}

	if( bind( sock, reinterpret_cast< SOCKADDR * >( &service ), sizeof( service ) ) == SOCKET_ERROR )
	{
		Log::Error( "bind() failed on port {}", port );
		closesocket( sock );
		return nullptr;
	}

	if( listen( sock, SOMAXCONN ) == SOCKET_ERROR )
	{
		Log::Error( "listen() failed on port {}", port );
		closesocket( sock );
		return nullptr;
	}

	auto realmSocket = std::make_shared< RealmSocket >();
	realmSocket->fd = sock;
	realmSocket->remote_ip = ip;
	realmSocket->remote_port = port;
	realmSocket->gameType = type;
	realmSocket->flag.is_listener = true;

	Log::Info( "Socket Opened on {}:{}", ip, port );

	return realmSocket;
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

		FD_SET( m_conSocket->fd, &readSet );
		FD_SET( m_rtaSocket->fd, &readSet );

		for( const auto &sock : m_gatewaySockets )
		{
			if( sock != nullptr && sock->fd != INVALID_SOCKET )
			{
				FD_SET( sock->fd, &readSet );
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

		for( auto &sock : m_gatewaySockets )
		{
			if( sock == nullptr || sock->fd == INVALID_SOCKET )
				continue;

			if( FD_ISSET( sock->fd, &readSet ) )
			{
				AcceptConnection( sock );
			}
		}

		if( FD_ISSET( m_conSocket->fd, &readSet ) )
		{
			AcceptConnection( m_conSocket );
		}

		if( FD_ISSET( m_rtaSocket->fd, &readSet ) )
		{
			AcceptConnection( m_rtaSocket );
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
			Log::Info( "[LOBBY] Client Timeout : ({})", socket->remote_ip );
		}

		// Check if we're waiting to disconnect after sending buffered data
		if( socket->flag.disconnected_wait && socket->m_pendingWriteBuffer.empty() )
		{
			socket->flag.disconnected_forced = true;
		}

		if( socket->flag.disconnected_forced )
		{
			UserManager::Get().RemoveUser( socket );
			Log::Info( "[LOBBY] Client Disconnected : ({})", socket->remote_ip );
			it = m_clientSockets.erase( it );
		}
		else
		{
			++it;
		}
	}
}

void LobbyServer::AcceptConnection( sptr_socket srcSocket )
{
	sockaddr_in clientInfo{};
	int32_t addrSize = sizeof( clientInfo );

	SOCKET clientSocket = accept( srcSocket->fd, ( SOCKADDR * )&clientInfo, &addrSize );
	if( clientSocket == INVALID_SOCKET )
	{
		Log::Error( "accept() failed" );
		return;
	}

	auto new_socket = std::make_shared< RealmSocket >();
	auto gameType = srcSocket->gameType;

	new_socket->fd = clientSocket;
	new_socket->remote_addr = clientInfo;
	new_socket->remote_ip = Util::IPFromAddr( clientInfo );
	new_socket->remote_port = ntohs( clientInfo.sin_port );
	new_socket->gameType = gameType;
	m_clientSockets.push_back( new_socket );

	if( !srcSocket->flag.is_gateway )
	{
		UserManager::Get().CreateUser( new_socket, gameType );
	}

	Log::Info( "New Client Connected : ({})", new_socket->remote_ip );
}

void LobbyServer::ReadSocket( sptr_socket socket )
{
	if( socket->flag.disconnected_forced )
	{
		return;
	}

	const auto bytesReceived = recv( socket->fd, reinterpret_cast< char * >( m_recvBuffer.data() ), static_cast< int >( m_recvBuffer.size() ), 0 );

	if( bytesReceived == SOCKET_ERROR )
	{
		Log::Info( "Socket Error [{}].", WSAGetLastError() );
		socket->flag.disconnected_forced = true;
		return;
	}

	if( bytesReceived == 0 )
	{
		socket->flag.disconnected_forced = true;
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
			Log::Error( "Invalid packet size: {}. Disconnecting client.", packetSize );
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
	if( socket->flag.disconnected_forced || socket->m_pendingWriteBuffer.empty() )
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

			Log::Error( "Send failed: {}", err );
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

	auto it = REQUEST_EVENT.find( packetId );
	if( it == REQUEST_EVENT.end() )
	{
		Log::Error( "[LOBBY] Unknown packet id : {}", packetId );
		Log::Packet( stream->m_buffer, stream->m_buffer.size(), false );
		return;
	}

	auto request = it->second();

	if( auto res = request->ProcessRequest( socket, stream ) )
	{
		socket->send( res );
	}
}