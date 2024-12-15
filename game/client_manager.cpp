#include "../global_define.h"

// Spawn a new client
bool CClientManager::spawn_new( sptr_socket socket, sptr_client &ret )
{
	sptr_client client = std::make_shared< CClient >();
	client->socket = socket;

	// The result if the 'insert'
	std::pair< std::map< SOCKET, sptr_client >::iterator, bool > result;

	// Insert the client into the map - keyed by its socket.
	result = client_map.insert( std::make_pair( socket->fd, client ) );

	ret = ( *result.first ).second;

	return result.second;
}

// Find an existing client in the service
bool CClientManager::get_client( sptr_socket socket, sptr_client &ret )
{
	std::map< SOCKET, sptr_client >::iterator it;
	if( client_map.end() == ( it = client_map.find( socket->fd ) ) ) return false;

	ret = ( *it ).second;
	return true;
}

// Find and remove a client by its socket
void CClientManager::remove_client( sptr_socket socket )
{
	std::map< SOCKET, sptr_client >::iterator it;
	if( client_map.end() == ( it = client_map.find( socket->fd ) ) ) return;

	client_map.erase( it );
}

// Remove a client and mark the socket for disconnection
void CClientManager::disconnect( sptr_client c )
{
	std::map< SOCKET, sptr_client >::iterator it;
	if( client_map.end() == ( it = client_map.find( c->socket->fd ) ) ) return;

	c->socket->flag.disconnected = 1;

	client_map.erase( it );
}