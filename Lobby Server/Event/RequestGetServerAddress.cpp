#include "../../global_define.h"
#include "RequestGetServerAddress.h"

void RequestGetServerAddress::Deserialize( sptr_byte_stream stream )
{
	DeserializeHeader( stream );
}

sptr_generic_response RequestGetServerAddress::ProcessRequest( sptr_user user, sptr_byte_stream stream )
{
	Deserialize( stream );

	return std::make_shared< ResultGetServerAddress >( this, Config::service_ip, Config::lobby_port );
}

ResultGetServerAddress::ResultGetServerAddress( GenericRequest *request, std::string ip, int32_t port ) : GenericResponse( *request )
{
	m_ip = ip;
	m_port = port;
}

ByteStream &ResultGetServerAddress::Serialize()
{
	m_stream.write_u16( m_packetId );
	m_stream.write_u32( m_requestId );
	m_stream.write_u32( 0 );

	m_stream.write_sz_utf8( m_ip );
	m_stream.write( m_port );

	return m_stream;
}
