#pragma once

class RealmUser {
public:
	RealmUser();
	~RealmUser();

	enum class UserState {
		MainMenu,
		JoinPending,
		InGameLobby,
		InGameSession,
	} m_state;

	sptr_tcp_socket m_realmSocket;
	sptr_udp_socket m_discoverySocket;
	std::wstring m_sessionId;
};

typedef std::shared_ptr< RealmUser >	sptr_user;
