// ╔╗╔╔═╗╦═╗╦═╗╔═╗╔╦╗╦ ╦               
// ║║║║ ║╠╦╝╠╦╝╠═╣ ║ ╠═╣               
// ╝╚╝╚═╝╩╚═╩╚═╩ ╩ ╩ ╩ ╩               
// ╦  ╔═╗╔╗ ╔╗ ╦ ╦  ╔═╗╔═╗╦═╗╦  ╦╔═╗╦═╗
// ║  ║ ║╠╩╗╠╩╗╚╦╝  ╚═╗║╣ ╠╦╝╚╗╔╝║╣ ╠╦╝
// ╩═╝╚═╝╚═╝╚═╝ ╩   ╚═╝╚═╝╩╚═ ╚╝ ╚═╝╩╚═

#include "../global_define.h"

#include "EventLookup.h"
#include "LobbyServer.h"

LobbyServer::LobbyServer()
{
	m_running = false;
	m_listenSocket = INVALID_SOCKET;

	m_clientSockets.clear();
	m_recvBuffer.resize( 1024 );
}

LobbyServer::~LobbyServer()
{
}

void LobbyServer::Start( std::string ip, int32_t port )
{
	m_listenSocket = ::WSASocket( AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED );
	if( m_listenSocket == INVALID_SOCKET )
	{
		Log::Error( "WSASocket() failed" );
		return;
	}

	// Bind the socket
	sockaddr_in service;
	service.sin_family = AF_INET;
	service.sin_port = htons( port );
	service.sin_addr.s_addr = ADDR_ANY;

	if( bind( m_listenSocket, ( SOCKADDR * )&service, sizeof( service ) ) == SOCKET_ERROR )
	{
		Log::Error( "bind() failed" );
		return;
	}

	// Listen on the socket
	if( listen( m_listenSocket, SOMAXCONN ) == SOCKET_ERROR )
	{
		Log::Error( "listen() failed" );
		return;
	}

	// Start the server
	m_running = true;
	m_thread = std::thread( &LobbyServer::Run, this );

	Log::Info( "Lobby Server started on %s:%d", ip.c_str(), port );
}

void LobbyServer::Stop()
{
	m_running = false;
	if( m_thread.joinable() )
	{
		m_thread.join();
	}
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

		FD_SET( m_listenSocket, &readSet );

		// Process clients
		for( auto &client : m_clientSockets )
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

		if( FD_ISSET( m_listenSocket, &readSet ) )
		{
			AcceptNewClient();
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

void LobbyServer::AcceptNewClient()
{
	sockaddr_in clientInfo{};
	int32_t addrSize = sizeof( clientInfo );

	SOCKET clientSocket = accept( m_listenSocket, ( SOCKADDR * )&clientInfo, &addrSize );
	if( clientSocket == INVALID_SOCKET )
	{
		Log::Error( "accept() failed" );
		return;
	}

	auto new_socket = std::make_shared< RealmTCPSocket >();
	new_socket->fd = clientSocket;
	new_socket->remote_address = clientInfo;
	new_socket->peer_ip_address = inet_ntoa( clientInfo.sin_addr );

	m_clientSockets.push_back( new_socket );

	Log::Info( "[LOBBY] New client connected : (%s)", new_socket->peer_ip_address.c_str() );
}

void LobbyServer::ReadSocket( sptr_tcp_socket socket )
{
	if( socket->flag.disconnected )
	{
		return;
	}

	auto bytesReceived = recv( socket->fd, ( char * )m_recvBuffer.data(), m_recvBuffer.size(), 0 );

	if( bytesReceived == SOCKET_ERROR )
	{
		auto error = WSAGetLastError();
		Log::Info( "Socket Error [%d].", error );
		socket->flag.disconnected = true;
		return;
	}

	if( bytesReceived == 0 )
	{
		Log::Info( "Socket Disconnected." );
		socket->flag.disconnected = true;
		return;
	}

	// Append the received data to the sockets processing buffer.
	// There's definitely a more elegant way of handling data here,
	// but this is just easier for now.
	socket->m_pendingReadBuffer.insert( socket->m_pendingReadBuffer.end(), m_recvBuffer.begin(), m_recvBuffer.begin() + bytesReceived );

	// Handle valid packets in the buffer.
	while( socket->m_pendingReadBuffer.size() > 0 )
	{
		auto packetSize = htonl( *( int32_t * )&socket->m_pendingReadBuffer[ 0 ] );

		if( packetSize > socket->m_pendingReadBuffer.size() )
		{
			break;
		}

		//Log::Packet( socket->m_pendingReadBuffer, packetSize, false );

		auto stream = std::make_shared< ByteStream >( socket->m_pendingReadBuffer.data() + 4, packetSize - 4 );

		// Erase the packet from the buffer
		socket->m_pendingReadBuffer.erase( socket->m_pendingReadBuffer.begin(), socket->m_pendingReadBuffer.begin() + packetSize );

		// Process the packet
		HandleRequest( socket, stream );
	}
}

void LobbyServer::WriteSocket( sptr_tcp_socket socket )
{
	if( socket->flag.disconnected )
	{
		return;
	}

	if( socket->m_pendingWriteBuffer.empty() )
	{
		return;
	}

	size_t totalBytesSent = 0;

	//Log::Packet( socket->m_pendingWriteBuffer, socket->m_pendingWriteBuffer.size(), false );

	while( true )
	{
		auto chunkSize = std::min< int >( socket->m_pendingWriteBuffer.size(), 1024 );
		auto bytesSent = send( socket->fd, ( char * )socket->m_pendingWriteBuffer.data(), chunkSize, 0 );

		if( bytesSent == SOCKET_ERROR )
		{
			socket->flag.disconnected = true;
			return;
		}

		totalBytesSent += bytesSent;

		if( bytesSent < chunkSize )
		{
			break;
		}

		if( totalBytesSent == socket->m_pendingWriteBuffer.size() )
		{
			break;
		}
	}

	socket->m_pendingWriteBuffer.erase( socket->m_pendingWriteBuffer.begin(), socket->m_pendingWriteBuffer.begin() + totalBytesSent );
}

void LobbyServer::HandleRequest( sptr_tcp_socket socket, sptr_byte_stream stream )
{
	auto packetId = stream->read< uint16_t >();
	stream->set_position( 0 );
	
	auto it = LOBBY_REQUEST_EVENT.find( packetId );
	if( it == LOBBY_REQUEST_EVENT.end() )
	{
		Log::Error( "[LOBBY] Unknown packet id : 0x%04X", packetId );
		Log::Packet( stream->data, stream->data.size(), false );
		return;
	}
	
	Log::Debug( "[LOBBY] Request processed : 0x%04X", packetId );
	
	auto request = it->second();
	
	if( auto res = request->ProcessRequest( socket, stream ) )
	{
		socket->send( res );
	}
}