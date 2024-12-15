#pragma once

#include <map>

class CClientManager {
public:
	CClientManager()
	{
		client_map.clear();
	}

	~CClientManager()
	{
		client_map.clear();
	}

	// Add a new client
	bool spawn_new( sptr_socket socket, sptr_client &ret );

	// Fine an existing client in the service
	bool get_client( sptr_socket socket, sptr_client &ret );
	
	// Remove an existing client in the service
	void remove_client( sptr_socket socket );

	void disconnect( sptr_client client );

	std::map< SOCKET, sptr_client > client_map;
};