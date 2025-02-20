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

	sockaddr_in service;
	service.sin_family = AF_INET;
	service.sin_port = htons( port );

	if (ip == "0.0.0.0")
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
	int clientAddrLen = sizeof(clientAddr);

	while( m_running )
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		auto bytesReceived = recvfrom(m_socket, (char*)m_recvBuffer.data(), 1024, 0, (struct sockaddr*)&clientAddr, &clientAddrLen);

		if (bytesReceived == SOCKET_ERROR || bytesReceived < 4)
		{
			continue;
		}

		ProcessPacket(&clientAddr, std::make_shared< ByteStream >(m_recvBuffer.data(), bytesReceived));
	}
}

void DiscoveryServer::ProcessPacket(sockaddr_in* clientAddr, sptr_byte_stream stream)
{
	Log::Packet( stream->data, stream->data.size(), false );

	auto head = stream->read_u32();

	if (0x20 == head)
	{
		ProcessDiscoveryInitial(clientAddr, stream);
	}

	auto user = RealmUserManager::Get().GetUserByAddress(clientAddr);

	if (nullptr == user)
	{
		return;
	}

	stream->set_position(0);

	if (user->is_host)
	{
		ProcessHost(user, stream);
	}
	else
	{
		ProcessJoin(user, stream);
	}
}

void DiscoveryServer::ProcessDiscoveryInitial( sockaddr_in *clientAddr, sptr_byte_stream stream )
{
	auto encryptedBytes = stream->read_bytes( 0x20 );
	auto decryptedBytes = RealmCrypt::decryptSymmetric(encryptedBytes);

	std::wstring sessionId(16, L'\0');
	std::memcpy(sessionId.data(), decryptedBytes.data(), 32);

	if( sessionId.empty() || sessionId.size() != 16 )
	{
		Log::Error( "Invalid session id." );
		return;
	}

	auto remoteIp = inet_ntoa( clientAddr->sin_addr );
	auto remotePort = ntohs( clientAddr->sin_port );

	auto user = RealmUserManager::Get().GetUser(sessionId);
	if (user == nullptr)
	{
		Log::Error("User not found! [%S]", sessionId.c_str());
		return;
	}

	if (remoteIp != user->tcp->remote_ip)
	{
		Log::Error( "Discovery Handshake from invalid IP!" );
		return;
	}

	user->udp->fd = m_socket;
	user->udp->remote_addr = *clientAddr;
	user->udp->remote_ip = remoteIp;
	user->udp->remote_port = remotePort;

	user->discovery_addr = remoteIp;
	user->discovery_port = remotePort;

	//Log::Debug( "New Discovery Connection : [%S] %s:%d", sessionId.c_str(), remoteIp, remotePort );
}

void DiscoveryServer::ProcessHost(sptr_user user, sptr_byte_stream stream)
{
	static int cmd = 0;
	switch (user->discovery_state)
	{
		case DiscoveryState::Initial_Connect:
		{
			if (false == GameSessionManager::Get().SetGameOpen(user))
			{
				Log::Error("Failed to set game open! [%S]", user->session_id.c_str());
				return;
			}

			user->discovery_state = DiscoveryState::Host_Waiting;

			user->udp->send(stream);
		} break;

		case DiscoveryState::Host_Waiting:
		{
			Send_Tmp(&user->udp->remote_addr);
		} break;
	}
}

void DiscoveryServer::ProcessJoin(sptr_user user, sptr_byte_stream stream)
{
	switch (user->discovery_state)
	{
		case DiscoveryState::Initial_Connect:
		{
			if (false == GameSessionManager::Get().JoinGame(user))
			{
				Log::Error("Failed to join game session! [%S]", user->session_id.c_str());
				return;
			}

			user->discovery_state = DiscoveryState::Negotiating;
		} break;

		case DiscoveryState::Negotiating:
		{
			// Initialize Realm Server Client
			Send_07(user);
			user->discovery_state = DiscoveryState::Joining;
		} break;

		case DiscoveryState::Joining:
		{
			if (false == Process_0C(user, stream))
			{
				Log::Error("Failed to parse user character data...");
				return;
			}

			user->discovery_state = DiscoveryState::Joined;
		} break;

		case DiscoveryState::Joined:
		{
			Send_08(user);
		} break;
	}
}

void DiscoveryServer::PrepareJoinGame(sptr_user user)
{
	if (false == GameSessionManager::Get().JoinGame(user))
	{

		Log::Error("Failed to join game session! [%S]", user->session_id.c_str());
		//return;
	}

	// Realm Server Client Initialize
	Send_07( user );

	user->discovery_state = DiscoveryState::Initial_Connect;
}

void DiscoveryServer::PrepareHostGame(sptr_user user)
{
	if (false == GameSessionManager::Get().SetGameOpen(user))
	{
		return;
	}

	user->discovery_state = DiscoveryState::Host_Waiting;
}

void DiscoveryServer::Send_07( sptr_user user )
{
	ByteStream reply;
	reply.write_u32(0x07);

	user->udp->send(reply);
}

void DiscoveryServer::Send_08( sptr_user user )
{
	auto session = GameSessionManager::Get().FindGame(user->game_id);

	if (nullptr == session)
	{
		Log::Error("Game session not found! [%d]", user->game_id);
		return;
	}

	ByteStream reply;
	{
		reply.write_u32(0x08);
		reply.write_sz_utf8("", 24);

		reply.write_sz_utf8(std::to_string(session->m_userList.size()));
		reply.write_sz_utf8("4");

		for (auto& member : session->m_userList)
		{
			reply.write_sz_utf8(member->player_name, 24);
			reply.write_sz_utf8(member->player_class, 24);
			reply.write_u32(member->player_level);
		}
	}
	
	for( auto &member : session->m_userList )
	{
		if (member->is_host)
			continue;

		if( !member->is_ready )
			continue;

		member->udp->send(reply);
	}
}

void DiscoveryServer::Send_09( sptr_user user )
{
	auto session = GameSessionManager::Get().FindGame( user->game_id );

	if( nullptr == session)
	{
		Log::Error( "Game session not found! [%d]", user->game_id);
		return;
	}

	ByteStream reply;
	{
		reply.write_u32(0x09);

		reply.write_u32(session->m_userList.size());

		for( auto &member : session->m_userList )
		{
			reply.write_sz_utf8(member->udp->remote_ip, 24);
			reply.write_u32(member->udp->remote_port);
		}
	}

	for( auto &member : session->m_userList )
	{
		member->udp->send(reply);
	}
}

bool DiscoveryServer::Process_0C( sptr_user user, sptr_byte_stream stream)
{
	if (0x0C != stream->read_u32())
	{
		return false;
	}

	//Log::Packet(stream->data, stream->data.size(), false);

	if (nullptr == user)
	{
		return false;
	}

	user->local_addr = stream->read_utf8(24);
	user->discovery_addr = stream->read_utf8(24);
	user->discovery_port = stream->read_u32();

	user->player_name = stream->read_utf8(24);
	user->player_class = stream->read_utf8(24);
	user->player_level = stream->read_u32();

	Log::Debug("Join User Info : %s %s %d", user->player_name.c_str(), user->player_class.c_str(), user->player_level);

	user->is_ready = true;

	Send_08(user);
	Send_0C(user);

	return true;
}

void DiscoveryServer::Send_0C( sptr_user user )
{
	auto session = GameSessionManager::Get().FindGame(user->game_id);

	if (nullptr == session)
	{
		Log::Error("Game session not found! [%d]", user->game_id);
		return;
	}

	auto host = session->GetHost();

	ByteStream reply;
	{
		reply.write_u32(0x0C);

		reply.write_sz_utf8(user->local_addr, 24);
		reply.write_sz_utf8(user->discovery_addr, 24);
		reply.write_u32(user->discovery_port);

		reply.write_sz_utf8(host->player_name, 24);
		reply.write_sz_utf8(host->player_class, 24);
		reply.write_u32(host->player_level);
	}
	host->udp->send(reply);
}

void DiscoveryServer::Send_0D( sptr_user user )
{
	// Send Reply to clientAddr
	std::vector< uint8_t > reply = { 0x0D, 0x00, 0x00, 0x00 };
}

void DiscoveryServer::Send_Tmp(sockaddr_in* clientAddr)
{
	ByteStream reply;
	{
		reply.write_u32(0x08);
		reply.write_sz_utf8("193.149.164.146", 24);
		reply.write_u32(10101);
	}

	sendto(m_socket, (char*)reply.data.data(), reply.data.size(), 0, (struct sockaddr*)clientAddr, sizeof(sockaddr_in));
}