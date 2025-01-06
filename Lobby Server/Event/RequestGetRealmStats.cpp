#include "../../global_define.h"
#include "RequestGetRealmStats.h"

void RequestGetRealmStats::Deserialize( sptr_tcp_socket socket, sptr_byte_stream stream )
{
	DeserializeHeader( stream );
}

sptr_generic_response RequestGetRealmStats::ProcessRequest( sptr_tcp_socket socket, sptr_byte_stream stream )
{
	Deserialize( socket, stream );

	return std::make_shared< ResultGetRealmStats >( this );
}

ResultGetRealmStats::ResultGetRealmStats( GenericRequest *request ) : GenericResponse( *request )
{
	
}

ByteStream &ResultGetRealmStats::Serialize()
{
	m_stream.write_u16( m_packetId );
	m_stream.write_u32( m_requestId );
	m_stream.write_u32( 0 );

	m_stream.write_u32( 1 );
	m_stream.write_u32( 2 );
	m_stream.write_u32( 3 );
	m_stream.write_u32( 4 );
	m_stream.write_u32( 5 );
	m_stream.write_u32( 6 );
	m_stream.write_u32( 7 );
	m_stream.write_u32( 8 );
	return m_stream;
}
