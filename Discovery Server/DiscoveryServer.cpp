// ╔╗╔╔═╗╦═╗╦═╗╔═╗╔╦╗╦ ╦                          
// ║║║║ ║╠╦╝╠╦╝╠═╣ ║ ╠═╣                          
// ╝╚╝╚═╝╩╚═╩╚═╩ ╩ ╩ ╩ ╩                          
// ╔╦╗╦╔═╗╔═╗╔═╗╦  ╦╔═╗╦═╗╦ ╦  ╔═╗╔═╗╦═╗╦  ╦╔═╗╦═╗
//  ║║║╚═╗║  ║ ║╚╗╔╝║╣ ╠╦╝╚╦╝  ╚═╗║╣ ╠╦╝╚╗╔╝║╣ ╠╦╝
// ═╩╝╩╚═╝╚═╝╚═╝ ╚╝ ╚═╝╩╚═ ╩   ╚═╝╚═╝╩╚═ ╚╝ ╚═╝╩╚═


#include "../global_define.h"
#include "../Lobby Server/Event/NotifyClientReqConnect.h"

DiscoveryServer::DiscoveryServer()
{
	m_running = false;

	m_sessionMap.clear();
	m_recvBuffer.resize( 1024 );
}

DiscoveryServer::~DiscoveryServer()
{
	Log::Info( "Discovery Server stopped." );
}

void DiscoveryServer::Start( std::string ip, int32_t port )
{
	if( false == OpenDiscoverySocket( ip, port ) )
	{
		Log::Error( "Failed to open discovery socket." );
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
	FD_SET readSet;
	FD_SET writeSet;

	timeval timeout = { 0, 1000 };

	m_timer.Start();

	while( m_running )
	{
		FD_ZERO( &readSet );
		FD_ZERO( &writeSet );

		FD_SET( m_socket->fd, &readSet );

		auto it = m_sessionMap.begin();
		for( it; it != m_sessionMap.end(); )
		{
			auto &socket = it->second->m_socket;

			//if( ::GetTickCount64() - socket->last_recv_time > 5001 )
			//{
			//	Log::Debug( "Disconnect timeout socket." );
			//	it = m_sessionMap.erase( it );
			//	continue;
			//}

			if( socket->m_pendingWriteQueue.empty() )
			{
				it++;
				continue;
			}

			FD_SET( socket->fd, &writeSet );

			it++;
		}

		auto result = select( 0, &readSet, &writeSet, nullptr, &timeout );

		if( result == SOCKET_ERROR )
		{
			std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );
			break;
		}

		if( FD_ISSET( m_socket->fd, &readSet ) )
		{
			ReadSocket( m_socket );
		}

		for( auto &session : m_sessionMap )
		{
			auto &socket = session.second->m_socket;

			if( FD_ISSET( socket->fd, &writeSet ) )
			{
				WriteSocket( socket );
			}
		}
	}
}

bool DiscoveryServer::OpenDiscoverySocket( std::string ip, int32_t port )
{
	SOCKET fd = socket( AF_INET, SOCK_DGRAM, 0 );

	if( fd == INVALID_SOCKET )
	{
		Log::Error( "Failed to create socket." );
		return false;
	}

	sockaddr_in serverInfo;
	serverInfo.sin_family = AF_INET;
	serverInfo.sin_addr.s_addr = inet_addr( ip.c_str() );
	serverInfo.sin_port = htons( port );

	if( bind( fd, ( SOCKADDR * )&serverInfo, sizeof( serverInfo ) ) == SOCKET_ERROR )
	{
		Log::Error( "Failed to bind socket." );
		closesocket( fd );
		return false;
	}

	m_socket = std::make_shared< RealmUDPSocket >();
	m_socket->local_address = serverInfo;

	m_socket->fd = fd;
	m_socket->port = port;
	m_socket->flag.is_listener = true;

	return true;
}

sptr_udp_socket DiscoveryServer::CreateDiscoverySocket( sockaddr_in *remoteAddr )
{
	auto socket = std::make_shared< RealmUDPSocket >();

	socket->fd = m_socket->fd;

	socket->local_address = m_socket->local_address;
	socket->remote_address = *remoteAddr;
	socket->peer_ip_address = inet_ntoa( remoteAddr->sin_addr );
	socket->peer_port = ntohs( remoteAddr->sin_port );

	socket->last_recv_time = ::GetTickCount64();

	Log::Debug( "Create new session socket for %s:%d", socket->peer_ip_address.c_str(), socket->peer_port );

	return socket;
}

sptr_discovery_record DiscoveryServer::CreateNewDiscoveryRecord( sockaddr_in *clientAddr, std::wstring sessionId )
{
	auto user = RealmUserManager::Get().GetUser( sessionId );

	if( user == nullptr )
	{
		Log::Error( "User not found for discovery! [%S]", sessionId.c_str() );
		return nullptr;
	}

	auto discoverySocket = CreateDiscoverySocket( clientAddr );
	auto session = DiscoverySession::Create( discoverySocket );

	session->m_ownerSessionId = sessionId;

	m_sessionMap[ *clientAddr ] = session;

	user->m_discoverySocket = discoverySocket;

	Log::Debug( "Create new session for %s:%d", inet_ntoa( clientAddr->sin_addr ), clientAddr->sin_port );

	return session;
}

sptr_discovery_record DiscoveryServer::GetDiscoveryRecord( sockaddr_in *clientAddr )
{
	auto it = m_sessionMap.find( *clientAddr );

	if( it == m_sessionMap.end() )
	{
		return nullptr;
	}

	return it->second;
}

std::wstring DiscoveryServer::GetSessionId( sptr_byte_stream stream )
{
	uint32_t length = stream->read<uint32_t>();
	auto decryptedBuffer = stream->read_encrypted_bytes( length );

	std::wstring sessionId( length, '\0' );

	std::memcpy( sessionId.data(), decryptedBuffer.data(), decryptedBuffer.size() );

	return sessionId;
}


void DiscoveryServer::AcceptNewClient( sockaddr_in *clientAddr, sptr_byte_stream stream )
{
	if( stream->data.size() < 36 )
	{
		Log::Error( "Invalid Discovery Handshake Packet" );
		return;
	}

	//auto sessionId = GetSessionId( stream );
	auto sessionId = stream->read_encrypted_utf16( false );

	if( sessionId.empty() || sessionId.size() != 16 )
	{
		Log::Error( "Invalid session id." );
		return;
	}

	auto record = CreateNewDiscoveryRecord( clientAddr, sessionId );

	if( record == nullptr )
	{
		Log::Error( "Failed to create new discovery record." );
		return;
	}

	auto game = GameSessionManager::Get().FindGame( 0 );
	auto &roomOwner = game->m_userList[ 0 ];

	auto ipAddr = inet_ntoa( clientAddr->sin_addr );
	auto port = ntohs( clientAddr->sin_port );

	NotifyClientRequestConnect msg( ipAddr, port );
	roomOwner->m_realmSocket->send( msg.Serialize() );
	//auto &discovery = DiscoveryServer::Get();

	SendDiscoveryClientHandshake( record, stream );
}

void DiscoveryServer::ReadSocket( sptr_udp_socket socket )
{
	if( socket->flag.disconnected )
	{
		return;
	}

	sockaddr_in clientAddr;
	int clientAddrLen = sizeof( clientAddr );

	// Receive data from the client
	auto bytesReceived = recvfrom( socket->fd, ( char * )m_recvBuffer.data(), ( int )m_recvBuffer.size(), 0, ( struct sockaddr * )&clientAddr, &clientAddrLen );

	if( bytesReceived == SOCKET_ERROR )
	{
		return;
	}

	if( bytesReceived == 0 )
	{
		return;
	}

	if( bytesReceived > 0 )
	{
		socket->last_recv_time = m_timer.GetElapsedTimeMilliseconds();

		HandleRequest( &clientAddr, std::make_shared< ByteStream >( m_recvBuffer.data(), bytesReceived ) );
	}
}

void DiscoveryServer::WriteSocket( sptr_udp_socket socket )
{
	if( socket->flag.disconnected )
	{
		return;
	}

	if( socket->m_pendingWriteQueue.empty() )
	{
		return;
	}

	auto &stream = socket->m_pendingWriteQueue.front();

	auto bytesSent = sendto( socket->fd, ( char * )stream->data.data(), ( int )stream->data.size(), 0, ( struct sockaddr * )&socket->remote_address, sizeof(socket->remote_address));

	if( bytesSent == SOCKET_ERROR )
	{
		return;
	}

	if( bytesSent > 0 )
	{
		socket->m_pendingWriteQueue.pop();
	}
}

void DiscoveryServer::HandleRequest( sockaddr_in *clientAddr, sptr_byte_stream stream )
{
	auto record = GetDiscoveryRecord( clientAddr );

	if( record == nullptr )
	{
		AcceptNewClient( clientAddr, stream );
		return;
	}

	auto remoteIp = inet_ntoa( clientAddr->sin_addr );
	auto remotePort = clientAddr->sin_port;

	Log::Debug( "%s:%d", remoteIp, remotePort );

	auto state = stream->read_u32();

	switch( state )
	{
		case 12:
		{
			SendDiscoveryClientPing( record, stream );
		} break;

		default:
		{
			Log::Packet( stream->data, stream->data.size(), false );
		} break;
	}
}

void DiscoveryServer::SendDiscoveryClientHandshake( sptr_discovery_record record, sptr_byte_stream stream )
{
	ByteStream response;
	response.write_u8( 0x07 ); //	REALM_SERVER_CLIENT_INITIAL

	record->m_socket->send( response );
}

void DiscoveryServer::SendDiscoveryClientPing( sptr_discovery_record record, sptr_byte_stream stream )
{
	auto remoteIp = stream->read_sz_utf8();

	ByteStream response;
	response.write_u8( 0x08 ); //	REALM_SERVER_CLIENT_PING

	record->m_socket->send( response );
}
