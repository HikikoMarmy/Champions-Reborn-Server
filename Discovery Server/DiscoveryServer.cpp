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

	sockaddr_in serverInfo;
	serverInfo.sin_family = AF_INET;
	serverInfo.sin_addr.s_addr = ADDR_ANY;// inet_addr( ip.c_str() );
	serverInfo.sin_port = htons( port );

	if( bind( m_socket, ( SOCKADDR * )&serverInfo, sizeof( serverInfo ) ) == SOCKET_ERROR )
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
	FD_SET readSet;
	timeval timeout = { 0, 1000 };

	while( m_running )
	{
		FD_ZERO( &readSet );
		FD_SET( m_socket, &readSet );

		auto result = select( 0, &readSet, nullptr, nullptr, &timeout );

		if( result == SOCKET_ERROR )
		{
			std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );
			break;
		}

		if( FD_ISSET( m_socket, &readSet ) )
		{
			ReadSocket();
		}
	}
}

void DiscoveryServer::UpdateDiscoveryInfo( std::wstring sessionId, std::string ip, int32_t port )
{
	if( sessionId.empty() || sessionId.size() != 16 )
	{
		Log::Error( "Invalid session id." );
		return;
	}

	m_userDiscoveryInfo[ sessionId ] = { ip, port, GetTickCount64() };

	Log::Debug( "UpdateDiscoveryInfo : [%S] %s:%d", sessionId.c_str(), ip.c_str(), port );
}

std::optional<UserDiscoveryInfo> DiscoveryServer::GetDiscoveryInfo( const std::wstring &sessionId ) const
{
	auto it = m_userDiscoveryInfo.find( sessionId );
	if( it == m_userDiscoveryInfo.end() )
	{
		return std::nullopt;
	}

	return it->second;
}

void DiscoveryServer::ReadSocket()
{
	sockaddr_in clientAddr;
	int clientAddrLen = sizeof( clientAddr );

	// Receive data from the client
	auto bytesReceived = recvfrom( m_socket, ( char * )m_recvBuffer.data(), ( int )m_recvBuffer.size(), 0, ( struct sockaddr * )&clientAddr, &clientAddrLen );

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
		HandleDiscoveryUpdate( &clientAddr, std::make_shared< ByteStream >( m_recvBuffer.data(), bytesReceived ) );
	}
}

void DiscoveryServer::HandleDiscoveryUpdate( sockaddr_in *clientAddr, sptr_byte_stream stream )
{
	if( stream->get_length() != 52 )
	{
		Log::Error( "Invalid discovery update." );
		return;
	}

	auto sessionId = stream->read_encrypted_utf16( false );

	if( sessionId.empty() || sessionId.size() != 16 )
	{
		Log::Error( "Invalid session id." );
		return;
	}

	auto remoteIp = inet_ntoa( clientAddr->sin_addr );
	auto remotePort = clientAddr->sin_port;

	//RealmUserManager::Get().UpdateUserDiscoveryInfo( sessionId, remoteIp, remotePort );

	UpdateDiscoveryInfo( sessionId, remoteIp, remotePort );
}

/*
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
*/