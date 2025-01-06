#pragma once

class RealmUser {
public:
	RealmUser();
	~RealmUser();

	enum class UserState {
		MainMenu,
		DiscoveryPending,
		JoinPending,
		InGameLobby,
		InGameSession,
	} m_state;

	sptr_tcp_socket m_realmSocket;
	
	std::wstring m_sessionId;
};

typedef std::shared_ptr< RealmUser >	sptr_user;
