#include "../../global_define.h"
#include "GetServerAddressEvent.h"

void RequestGetServerAddress::Deserialize( sptr_tcp_socket socket, sptr_byte_stream stream )
{
	DeserializeHeader( stream );
}

sptr_generic_response RequestGetServerAddress::ProcessRequest( sptr_tcp_socket socket, sptr_byte_stream stream )
{
	Deserialize( socket, stream );

	return std::make_shared< ResultGetServerAddress >( this, "192.168.1.248", 40810 );
}

ResultGetServerAddress::ResultGetServerAddress( GenericRequest *request, std::string ip, int32_t port ) : GenericResponse( *request )
{
	m_ip = ip;
	m_port = port;
}

ByteStream& ResultGetServerAddress::Serialize()
{
	m_stream.write_u16( m_packetId );
	m_stream.write_u32( m_requestId );
	m_stream.write_u32( 0 );

	m_stream.write_sz_utf8( m_ip );
	m_stream.write( m_port );

	return m_stream;
}
